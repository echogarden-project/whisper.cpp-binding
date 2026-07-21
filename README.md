# Whisper.cpp native binding for Echogarden

This is a minimal, low-level Node.js binding for the `whisper.cpp` library, used internally by the `whisper` engine in [Echogarden](https://github.com/echogarden-project/echogarden) `v3.0.0` or newer.

It exposes only the absolute minimum operations it needs for inference:

* Encode a Mel spectrogram using the Whisper model
* Decode a token using the Whisper model
* Get logits for last decoded token
* Get cross-attention query-key tensors (internal model data used for generating accurate timestamps)

There are no tokenization, signal processing, token sampling, timing computations, etc. operations exposed. Echogarden uses its own methods for them.

It requires a special, modified version of the `whisper.cpp` codebase, since cross-attention QKs are not exposed in its standard C API.
