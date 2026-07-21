//////////////////////////////////////////////////////////////////////////////////////////////
// This is auto-generated code for loading and wrapping a dynamically linked library
// at runtime via 'dlopen' (Linux / macOS) or 'LoadLibrary' (Windows).
//
// It was generated via a TypeScript utility, for a particular set of given
// imported symbol names.
//
// It shouldn't be edited manually.
//////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "whisper.h"

#if defined(_WIN32)
#include <windows.h> // For LoadLibrary, GetProcAddress, FreeLibrary, HMODULE
#define RESOLVE_SYMBOL GetProcAddress
#else
#include <dlfcn.h> // For dlopen, dlsym, dlclose
#define RESOLVE_SYMBOL dlsym
#endif

#include <string>
#include <stdexcept>

//////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper class 'WhisperCppDynamicLib'
//////////////////////////////////////////////////////////////////////////////////////////////
class WhisperCppDynamicLib {
private:
#if defined(_WIN32)
	HMODULE _libHandle = nullptr;
#else
	void* _libHandle = nullptr;
#endif

public:
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Declare exported symbols as class methods, preserving their types:
	//////////////////////////////////////////////////////////////////////////////////////////////
	decltype(&::whisper_context_default_params) whisper_context_default_params = nullptr;
	decltype(&::whisper_init_from_file_with_params) whisper_init_from_file_with_params = nullptr;
	decltype(&::whisper_init_state) whisper_init_state = nullptr;
	decltype(&::whisper_pcm_to_mel_with_state) whisper_pcm_to_mel_with_state = nullptr;
	decltype(&::whisper_encode_with_state) whisper_encode_with_state = nullptr;
	decltype(&::whisper_set_mel_with_state) whisper_set_mel_with_state = nullptr;
	decltype(&::whisper_decode_with_state) whisper_decode_with_state = nullptr;
	decltype(&::whisper_get_logits_from_state) whisper_get_logits_from_state = nullptr;
	decltype(&::whisper_get_aheads_cross_QKs_dims) whisper_get_aheads_cross_QKs_dims = nullptr;
	decltype(&::whisper_write_aheads_cross_QKs) whisper_write_aheads_cross_QKs = nullptr;
	decltype(&::whisper_n_vocab) whisper_n_vocab = nullptr;
	decltype(&::whisper_free_state) whisper_free_state = nullptr;
	decltype(&::whisper_free) whisper_free = nullptr;
	decltype(&::whisper_log_set) whisper_log_set = nullptr;

	WhisperCppDynamicLib(const std::string& libFilePath) {
		//////////////////////////////////////////////////////////////////////////////////////////////
		// Load dynamically linked library:
		//////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
		_libHandle = LoadLibraryExA(libFilePath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
		_libHandle = dlopen(libFilePath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#endif

		if (!_libHandle) {
#if defined(_WIN32)
			throw std::runtime_error("Couldn't load dynamic library '" + libFilePath + "' (Windows Error Code: " + std::to_string(GetLastError()) + ")");
#else
			auto dlErrString = dlerror();
			std::string errMessage;

			if (dlErrString != nullptr) {
				errMessage = std::string(dlErrString);
			} else {
				errMessage = "unknown error";
			}

			throw std::runtime_error("Couldn't load dynamic library '" + libFilePath + "': " + errMessage);
#endif
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_context_default_params':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_context_default_params = reinterpret_cast<decltype(&::whisper_context_default_params)>(RESOLVE_SYMBOL(_libHandle, "whisper_context_default_params"));

		if (!whisper_context_default_params) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_context_default_params' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_init_from_file_with_params':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_init_from_file_with_params = reinterpret_cast<decltype(&::whisper_init_from_file_with_params)>(RESOLVE_SYMBOL(_libHandle, "whisper_init_from_file_with_params"));

		if (!whisper_init_from_file_with_params) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_init_from_file_with_params' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_init_state':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_init_state = reinterpret_cast<decltype(&::whisper_init_state)>(RESOLVE_SYMBOL(_libHandle, "whisper_init_state"));

		if (!whisper_init_state) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_init_state' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_pcm_to_mel_with_state':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_pcm_to_mel_with_state = reinterpret_cast<decltype(&::whisper_pcm_to_mel_with_state)>(RESOLVE_SYMBOL(_libHandle, "whisper_pcm_to_mel_with_state"));

		if (!whisper_pcm_to_mel_with_state) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_pcm_to_mel_with_state' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_encode_with_state':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_encode_with_state = reinterpret_cast<decltype(&::whisper_encode_with_state)>(RESOLVE_SYMBOL(_libHandle, "whisper_encode_with_state"));

		if (!whisper_encode_with_state) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_encode_with_state' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_set_mel_with_state':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_set_mel_with_state = reinterpret_cast<decltype(&::whisper_set_mel_with_state)>(RESOLVE_SYMBOL(_libHandle, "whisper_set_mel_with_state"));

		if (!whisper_set_mel_with_state) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_set_mel_with_state' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_decode_with_state':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_decode_with_state = reinterpret_cast<decltype(&::whisper_decode_with_state)>(RESOLVE_SYMBOL(_libHandle, "whisper_decode_with_state"));

		if (!whisper_decode_with_state) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_decode_with_state' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_get_logits_from_state':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_get_logits_from_state = reinterpret_cast<decltype(&::whisper_get_logits_from_state)>(RESOLVE_SYMBOL(_libHandle, "whisper_get_logits_from_state"));

		if (!whisper_get_logits_from_state) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_get_logits_from_state' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_get_aheads_cross_QKs_dims':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_get_aheads_cross_QKs_dims = reinterpret_cast<decltype(&::whisper_get_aheads_cross_QKs_dims)>(RESOLVE_SYMBOL(_libHandle, "whisper_get_aheads_cross_QKs_dims"));

		if (!whisper_get_aheads_cross_QKs_dims) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_get_aheads_cross_QKs_dims' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_write_aheads_cross_QKs':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_write_aheads_cross_QKs = reinterpret_cast<decltype(&::whisper_write_aheads_cross_QKs)>(RESOLVE_SYMBOL(_libHandle, "whisper_write_aheads_cross_QKs"));

		if (!whisper_write_aheads_cross_QKs) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_write_aheads_cross_QKs' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_n_vocab':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_n_vocab = reinterpret_cast<decltype(&::whisper_n_vocab)>(RESOLVE_SYMBOL(_libHandle, "whisper_n_vocab"));

		if (!whisper_n_vocab) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_n_vocab' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_free_state':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_free_state = reinterpret_cast<decltype(&::whisper_free_state)>(RESOLVE_SYMBOL(_libHandle, "whisper_free_state"));

		if (!whisper_free_state) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_free_state' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_free':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_free = reinterpret_cast<decltype(&::whisper_free)>(RESOLVE_SYMBOL(_libHandle, "whisper_free"));

		if (!whisper_free) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_free' in dynamic library '" + libFilePath + "'.");
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Resolve symbol 'whisper_log_set':
		//////////////////////////////////////////////////////////////////////////////////////////////
		whisper_log_set = reinterpret_cast<decltype(&::whisper_log_set)>(RESOLVE_SYMBOL(_libHandle, "whisper_log_set"));

		if (!whisper_log_set) {
			_unload();
			throw std::runtime_error("Couldn't find symbol 'whisper_log_set' in dynamic library '" + libFilePath + "'.");
		}
	}

	~WhisperCppDynamicLib() {
		_unload();
	}

private:
	void _unload() {
		if (_libHandle) {
#if defined(_WIN32)
			FreeLibrary(_libHandle);
#else
			dlclose(_libHandle);
#endif
			_libHandle = nullptr;
		}
	}
};
