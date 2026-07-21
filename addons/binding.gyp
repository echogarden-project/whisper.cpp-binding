{
    "targets": [
        {
            # Common settings for all OSes and architectures
            "sources": ["src/WhisperCppWrapper.cpp"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
				"include/ggml/",
                "include/whisper.cpp/",
            ],
            "defines": ["NAPI_CPP_EXCEPTIONS"],
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "conditions": [
                # Windows specific settings
                [
                    "OS=='win'",
                    {
                        "libraries": [
                        ],
                        "msvs_settings": {
                            "VCCLCompilerTool": {
                                "ExceptionHandling": 1,
                                "AdditionalOptions": ["/std:c++20"],
                            },
                        },
                        "conditions": [
                            [
                                "target_arch=='x64'",
                                {
                                    "target_name": "whisper-cpp-wrapper-windows-x64",
                                },
                                "target_arch=='arm64'",
                                {
                                    "target_name": "whisper-cpp-wrapper-windows-arm64",
                                },
                            ],
                        ],
                    },
                ],
                # Linux specific settings
                [
                    "OS=='linux'",
                    {
                        "cflags_cc": ["-std=c++20"],
                        "ldflags": ["-Wl,-rpath,'$$ORIGIN'"],
                        "conditions": [
                            [
                                "target_arch=='x64'",
                                {
                                    "target_name": "whisper-cpp-wrapper-linux-x64",
                                    "libraries": [
                                    ],
                                },
                            ],
                            [
                                "target_arch=='arm64'",
                                {
                                    "target_name": "whisper-cpp-wrapper-linux-arm64",
                                    "libraries": [
                                    ],
                                    "append_cflags": ["-I ~/arm64-libs/usr/include"],
                                    "append_ldflags": [
                                        "-L ~/arm64-libs/usr/lib/aarch64-linux-gnu"
                                    ],
                                },
                            ],
                        ],
                    },
                ],
                # macOS specific settings
                [
                    "OS=='mac'",
                    {
                        "libraries": [
                        ],
                        "xcode_settings": {
                            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                            "CLANG_CXX_LANGUAGE_STANDARD": "c++20",
                            "OTHER_LDFLAGS": ["-rpath", "@loader_path"],
                        },
                        "conditions": [
                            [
                                "target_arch=='x64'",
                                {
                                    "target_name": "whisper-cpp-wrapper-macos-x64",
                                },
                            ],
                            [
                                "target_arch=='arm64'",
                                {
                                    "target_name": "whisper-cpp-wrapper-macos-arm64",
                                },
                            ],
                        ],
                    },
                ],
            ],
        },
    ],
}
