#include "../include/WhisperCppDynamicLib.h"
#include "../include/AsyncFunctionWorker.h"
#include "../include/Utilities.h"

#include <napi.h>
#include <string>
#include <map>

///////////////////////////////////////////////////////////////////////////////////////////
// NOTE:
//
// This N-API binding is not a library meant for independent, general usage.
// It's an integrated part of the Echogarden `whisper.cpp-binding` library.
// The code assumes that all arguments are correctly provided and are 100% valid!
// It's delibartely done for minimizing complexity on the C++ side, and ease of debugging.
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// Whisper.cpp NAPI context wrapper
///////////////////////////////////////////////////////////////////////////////////////////
class WhisperCppContextWrapper : public Napi::ObjectWrap<WhisperCppContextWrapper> {
   private:

	WhisperCppDynamicLib* lib = nullptr;
	whisper_context* context = nullptr;
	whisper_state* state = nullptr;

	bool isInitialized = false;

   public:
	WhisperCppContextWrapper(const Napi::CallbackInfo& info)
		: Napi::ObjectWrap<WhisperCppContextWrapper>(info) {}

	// Initialization
	Napi::Value Initialize(const Napi::CallbackInfo& info) {
		auto env = info.Env();

		// Read JavaScript arguments
		auto configObject = info[0].As<Napi::Object>();

		auto libFilePath = configObject.Get("libFilePath").As<Napi::String>().Utf8Value();
		auto modelFilePath = configObject.Get("modelFilePath").As<Napi::String>().Utf8Value();

		auto enableGPU = configObject.Get("enableGPU").As<Napi::Boolean>().Value();
		auto enableFlashAttention = configObject.Get("enableFlashAttention").As<Napi::Boolean>().Value();
		auto gpuDeviceIndex = configObject.Get("gpuDeviceIndex").As<Napi::Number>().Int32Value();

		auto alignmentHeadsPresetAsInt32 = configObject.Get("alignmentHeadsPreset").As<Napi::Number>().Int32Value();
		auto alignmentHeadsPreset = static_cast<whisper_alignment_heads_preset>(alignmentHeadsPresetAsInt32);
		auto alignmentHeadsTopCount = configObject.Get("alignmentHeadsTopCount").As<Napi::Number>().Int32Value();

		auto logLevel = configObject.Get("logLevel").As<Napi::Number>().Int32Value();

		// Initialize whisper.cpp dynamic library
		try {
			if (lib == nullptr) {
				lib = new WhisperCppDynamicLib(libFilePath);
			}
		} catch (const std::exception& e) {
			const char* errorMessage = e.what();

			if (errorMessage == nullptr) {
				errorMessage = "Unknown error occurred when loading whisper.cpp shared library.";
			}

			Napi::Error::New(env, errorMessage).ThrowAsJavaScriptException();

			return env.Undefined();
		}

		// Initialize context
		auto contextParams = lib->whisper_context_default_params();

		contextParams.use_gpu = enableGPU;
		contextParams.flash_attn = enableFlashAttention;
		contextParams.gpu_device = gpuDeviceIndex;

		contextParams.dtw_token_timestamps = alignmentHeadsPreset != WHISPER_AHEADS_NONE;
		contextParams.dtw_aheads_preset = alignmentHeadsPreset;
		contextParams.dtw_n_top = alignmentHeadsTopCount;

		//
		return RunAsync(this->Value(), env, [=, this]() {
			// Set logger
			auto whisperLogger = [](enum ggml_log_level level, const char* text, void* user_data) {
				int logLevel = *static_cast<int*>(user_data);

				if (level >= logLevel) {
					printf("[whisper.cpp] %s", text);
				}
			};

			auto logLevelPtr = new int32_t(logLevel);  // TODO: Ensure this is released when not needed

			lib->whisper_log_set(whisperLogger, logLevelPtr);

			// Initialize context
			context = lib->whisper_init_from_file_with_params(modelFilePath.c_str(), contextParams);

			// Initialize state
			if (context) {
				state = lib->whisper_init_state(context);
			}

			return [=, this](Napi::Env env) {
				if (!context) {
					delete lib;

					Napi::Error::New(env, "Failed to create context.").ThrowAsJavaScriptException();
				} else if (!state) {
					lib->whisper_free(context);
					delete lib;

					Napi::Error::New(env, "Failed to create state.").ThrowAsJavaScriptException();
				} else {
					isInitialized = true;
				}

				return env.Undefined();
			};
		});
	}

	Napi::Value EncodeSamples(const Napi::CallbackInfo& info) {
		auto env = info.Env();

		// Read JavaScript arguments
		auto samples = info[0].As<Napi::Float32Array>();
		auto threadCount = info[1].As<Napi::Number>().Int32Value();

		// Start
		return RunAsync(this->Value(), env, [=, this]() {
			int pcmToMelResultCode;

			pcmToMelResultCode = lib->whisper_pcm_to_mel_with_state(
				context, state, samples.Data(), samples.ElementLength(), threadCount);

			int encoderResultCode = -1;

			if (pcmToMelResultCode == 0) {
				encoderResultCode = lib->whisper_encode_with_state(context, state, 0, threadCount);
			}

			return [=](Napi::Env env) {
				if (pcmToMelResultCode != 0) {
					Napi::Error::New(env, "Failed to convert PCM to Mel spectrogram.")
						.ThrowAsJavaScriptException();
				}

				if (encoderResultCode != 0) {
					Napi::Error::New(env, "Failed to encode Mel spectrogram.")
						.ThrowAsJavaScriptException();
				}

				return env.Undefined();
			};
		});
	}

	Napi::Value EncodeLogMelSpectrogram(const Napi::CallbackInfo& info) {
		auto env = info.Env();

		auto melSpectrogram = info[0].As<Napi::Float32Array>();
		auto melCount = info[1].As<Napi::Number>().Int32Value();
		auto threadCount = info[2].As<Napi::Number>().Int32Value();

		// Start
		return RunAsync(this->Value(), env, [=, this]() {
			int setMelResultCode;

			setMelResultCode = lib->whisper_set_mel_with_state(
				context, state, melSpectrogram.Data(), melSpectrogram.ElementLength() / melCount, melCount);

			int encoderResultCode = -1;

			if (setMelResultCode == 0) {
				encoderResultCode = lib->whisper_encode_with_state(context, state, 0, threadCount);
			}

			return [=](Napi::Env env) {
				if (setMelResultCode != 0) {
					printf("Failed to set Mel spectrogram.\n");

					Napi::Error::New(env, "Failed to set Mel spectrogram.")
						.ThrowAsJavaScriptException();
				}

				if (encoderResultCode != 0) {
					printf("Failed to encode Mel spectrogram.\n");

					Napi::Error::New(env, "Failed to encode Mel spectrogram.")
						.ThrowAsJavaScriptException();
				}

				return env.Undefined();
			};
		});
	}

	Napi::Value DecodeTokens(const Napi::CallbackInfo& info) {
		auto env = info.Env();

		// Read JavaScript arguments
		auto tokens = info[0].As<Napi::Int32Array>();
		auto historyPrefixLength = info[1].As<Napi::Number>().Int32Value();
		auto threadCount = info[2].As<Napi::Number>().Int32Value();

		// Start
		return RunAsync(this->Value(), env, [=, this]() {
			auto resultCode = lib->whisper_decode_with_state(context, state, tokens.Data(),
															 tokens.ElementLength(),
															 historyPrefixLength, threadCount);

			return [=](Napi::Env env) {
				if (resultCode != 0) {
					Napi::Error::New(env, "Failed to decode tokens.").ThrowAsJavaScriptException();
				}

				return env.Undefined();
			};
		});
	}

	Napi::Value GetLogits(const Napi::CallbackInfo& info) {
		auto env = info.Env();

		auto outputLogits = info[0].As<Napi::Float32Array>();

		auto logits = lib->whisper_get_logits_from_state(state);

		memcpy(outputLogits.Data(), logits, outputLogits.ByteLength());

		return env.Undefined();
	}

	Napi::Value GetCrossAttentionQKsDimensions(const Napi::CallbackInfo& info) {
		auto env = info.Env();

		auto dimensions = lib->whisper_get_aheads_cross_QKs_dims(state);

		auto dimensionsJS = Napi::BigInt64Array::New(env, 3);
		memcpy(dimensionsJS.Data(), dimensions, 3 * sizeof(int64_t));

		delete[] dimensions;

		return dimensionsJS;
	}

	Napi::Value WriteCrossAttentionQKs(const Napi::CallbackInfo& info) {
		auto env = info.Env();

		auto outputData = info[0].As<Napi::Float32Array>();

		auto data = outputData.Data();

		return RunAsync(this->Value(), env, [=, this]() {
			lib->whisper_write_aheads_cross_QKs(state, data);

			return nullptr;
		});
	}

	Napi::Value GetVocabSize(const Napi::CallbackInfo& info) {
		auto env = info.Env();

		auto vocabSize = lib->whisper_n_vocab(context);

		return Napi::Number::From(env, vocabSize);
	}

	// This method is called from JavaScript only!
	//
	// It allows to immediately release memory for the instance.
	// The JavaScript caller must ensure that it never calls any other method after this.
	void Dispose(const Napi::CallbackInfo& info) { delete this; }

	~WhisperCppContextWrapper() {
		if (isInitialized) {
			lib->whisper_free_state(state);
			lib->whisper_free(context);
		}

		state = nullptr;
		context = nullptr;

		isInitialized = false;
	}

	static Napi::Object CreateNapiConstructor(Napi::Env env) {
		return DefineClass(
			env, "WhisperCppContextWrapper",
			{
				InstanceMethod("initialize", &WhisperCppContextWrapper::Initialize),
				InstanceMethod("encodeLogMelSpectrogram", &WhisperCppContextWrapper::EncodeLogMelSpectrogram),
				InstanceMethod("encodeSamples", &WhisperCppContextWrapper::EncodeSamples),
				InstanceMethod("decodeTokens", &WhisperCppContextWrapper::DecodeTokens),
				InstanceMethod("getLogits", &WhisperCppContextWrapper::GetLogits),
				InstanceMethod("getCrossAttentionQKsDimensions", &WhisperCppContextWrapper::GetCrossAttentionQKsDimensions),
				InstanceMethod("writeCrossAttentionQKs", &WhisperCppContextWrapper::WriteCrossAttentionQKs),
				InstanceMethod("getVocabSize", &WhisperCppContextWrapper::GetVocabSize),
				InstanceMethod("dispose", &WhisperCppContextWrapper::Dispose),
			});
	}
};

///////////////////////////////////////////////////////////////////////////////////////////
// Addon initialization
///////////////////////////////////////////////////////////////////////////////////////////
Napi::Object InitializeAddon(Napi::Env env, Napi::Object exports) {
	exports["WhisperCppContextWrapper"] = WhisperCppContextWrapper::CreateNapiConstructor(env);

	return exports;
}

NODE_API_MODULE(addon, InitializeAddon)
