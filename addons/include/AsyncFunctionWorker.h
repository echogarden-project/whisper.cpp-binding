#pragma once

#include <napi.h>

using ResultMakerFunc = std::function<Napi::Value(Napi::Env)>;
using NapiTaskFunc = std::function<ResultMakerFunc()>;

class AsyncFunctionWorker : public Napi::AsyncWorker {
   private:
	Napi::Reference<Napi::Object> ownerRef;
	Napi::Promise::Deferred deferred;
	NapiTaskFunc taskFunc;
	ResultMakerFunc resultMaker;

   public:
	explicit AsyncFunctionWorker(Napi::Object owner,
								 Napi::Promise::Deferred deferred,
								 NapiTaskFunc taskFunc)
		: Napi::AsyncWorker(deferred.Env()),
		  ownerRef(Napi::Persistent(owner)),
		  deferred(std::move(deferred)),
		  taskFunc(std::move(taskFunc)) {}

	void Execute() override {
		try {
			if (taskFunc) {
				resultMaker = taskFunc();
			} else {
				resultMaker = nullptr;
			}
		} catch (const std::exception& ex) {
			SetError(ex.what());
		} catch (...) {
			SetError("Unknown exception in task function.");
		}
	}

	void OnOK() override {
		try {
			if (resultMaker) {
				auto result = resultMaker(Env());
				deferred.Resolve(result);
			} else {
				deferred.Resolve(Env().Undefined());
			}
		} catch (const Napi::Error& ex) {
			// Handle Napi::Error exceptions (e.g., from ThrowAsJavaScriptException())
			deferred.Reject(ex.Value());
		} catch (const std::exception& ex) {
			deferred.Reject(Napi::Error::New(Env(), ex.what()).Value());
		} catch (...) {
			deferred.Reject(Napi::Error::New(Env(), "Unknown exception in result maker").Value());
		}
	}

	void OnError(const Napi::Error& error) override { deferred.Reject(error.Value()); }
};

Napi::Promise RunAsync(Napi::Object owner, Napi::Env env, const NapiTaskFunc& func) {
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

	AsyncFunctionWorker* worker = new AsyncFunctionWorker(owner, std::move(deferred), func);

	worker->Queue();

	return deferred.Promise();
}
