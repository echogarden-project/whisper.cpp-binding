import path from 'path'

import { extendDeep } from '../utilities/ObjectUtilities.js'

export class WhisperCppContext {
	modelFilePath: string | undefined

	private context: any

	async initialize(options: Partial<WhisperCppContextOptions>) {
		if (this.isInitialized) {
			throw new Error(`whisper.cpp context object is already initialized`)
		}

		if (!options.libFilePath) {
			throw new Error(`whisper.cpp shared library file path is not set in options`)
		}

		if (!options.modelFilePath) {
			throw new Error(`Model path is not set in options`)
		}

		options = extendDeep(defaultWhisperCppModelOptions, options)

		options.libFilePath = path.resolve(options.libFilePath!)
		options.modelFilePath = path.resolve(options.modelFilePath!)

		const addon = await getWhisperCppAddonForCurrentPlatform()

		const newContext = new addon.WhisperCppContextWrapper()

		await newContext.initialize(options)

		this.context = newContext
		this.modelFilePath = options.modelFilePath
	}

	async encodeSamples(samples: Float32Array, threadCount: number) {
		return this.context.encodeSamples(samples, threadCount)
	}

	async encodeLogMelSpectrogram(logMelSpectrogram: Float32Array, melCountPerFrame: number, threadCount = 4) {
		return this.context.encodeLogMelSpectrogram(logMelSpectrogram, melCountPerFrame, threadCount)
	}

	async decodeTokens(tokens: number[], historyPrefixLength: number, threadCount: number) {
		const tokensAsInt32Array = Int32Array.from(tokens)

		return this.context.decodeTokens(tokensAsInt32Array, historyPrefixLength, threadCount)
	}

	getLogits() {
		const vocabSize = this.getVocabSize()

		const logits = new Float32Array(vocabSize)

		this.context.getLogits(logits)

		return logits
	}

	async getCrossAttentionQKs() {
		const dimensions = this.context.getCrossAttentionQKsDimensions() as BigInt64Array
		const totalElementCount = Number(dimensions[0] * dimensions[1] * dimensions[2])

		const data = new Float32Array(totalElementCount)

		if (data.length !== 0) {
			await this.context.writeCrossAttentionQKs(data)
		}

		const result: CrossAttentionQKs = {
			dimensions,
			data,
		}

		return result
	}

	getVocabSize() {
		return this.context.getVocabSize() as number
	}

	release() {
		if (!this.isInitialized) {
			return
		}

		this.context.dispose()

		this.context = undefined
	}

	get isInitialized() { return this.context !== undefined }
}

let addonInstance: any

async function getWhisperCppAddonForCurrentPlatform() {
	if (addonInstance) {
		return addonInstance
	}

	const platform = process.platform
	const arch = process.arch

	const { default: NodeModule } = await import('node:module')
	const require = NodeModule.createRequire(import.meta.url)

	let addonPath: string

	if (platform === 'win32' && arch === 'x64') {
		addonPath = '../../addons/bin/whisper-cpp-wrapper-windows-x64.node'
	} else if (platform === 'win32' && arch === 'arm64') {
		addonPath = '../../addons/bin/whisper-cpp-wrapper-windows-arm64.node'
	} else if (platform === 'darwin' && arch === 'x64') {
		addonPath = '../../addons/bin/whisper-cpp-wrapper-macos-x64.node'
	} else if (platform === 'darwin' && arch === 'arm64') {
		addonPath = '../../addons/bin/whisper-cpp-wrapper-macos-arm64.node'
	} else if (platform === 'linux' && arch === 'x64') {
		addonPath = '../../addons/bin/whisper-cpp-wrapper-linux-x64.node'
	} else if (platform === 'linux' && arch === 'arm64') {
		addonPath = '../../addons/bin/whisper-cpp-wrapper-linux-arm64.node'
	} else {
		throw new Error(`Architecture ${platform}-${arch} is not supported by the whisper.cpp binding.`)
	}

	addonInstance = require(addonPath)

	return addonInstance
}

export enum WhisperAlignmentHeadsPreset {
	WHISPER_AHEADS_NONE,
	WHISPER_AHEADS_N_TOP_MOST,  // All heads from the N-top-most text-layers
	WHISPER_AHEADS_CUSTOM,
	WHISPER_AHEADS_TINY_EN,
	WHISPER_AHEADS_TINY,
	WHISPER_AHEADS_BASE_EN,
	WHISPER_AHEADS_BASE,
	WHISPER_AHEADS_SMALL_EN,
	WHISPER_AHEADS_SMALL,
	WHISPER_AHEADS_MEDIUM_EN,
	WHISPER_AHEADS_MEDIUM,
	WHISPER_AHEADS_LARGE_V1,
	WHISPER_AHEADS_LARGE_V2,
	WHISPER_AHEADS_LARGE_V3,
	WHISPER_AHEADS_LARGE_V3_TURBO,
}

export enum GgmlLogLevel {
	GGML_LOG_LEVEL_NONE = 0,
	GGML_LOG_LEVEL_DEBUG = 1,
	GGML_LOG_LEVEL_INFO = 2,
	GGML_LOG_LEVEL_WARN = 3,
	GGML_LOG_LEVEL_ERROR = 4,
	GGML_LOG_LEVEL_CONT = 5, // continue previous log
}

export interface WhisperCppContextOptions {
	libFilePath: string
	modelFilePath: string

	enableGPU: boolean
	enableFlashAttention: boolean
	gpuDeviceIndex: number

	alignmentHeadsPreset: WhisperAlignmentHeadsPreset
	alignmentHeadsTopCount: number

	logLevel: GgmlLogLevel
}

const defaultWhisperCppModelOptions: WhisperCppContextOptions = {
	libFilePath: undefined as any,
	modelFilePath: undefined as any,

	enableGPU: false,
	enableFlashAttention: false,
	gpuDeviceIndex: 0,

	alignmentHeadsPreset: WhisperAlignmentHeadsPreset.WHISPER_AHEADS_NONE,
	alignmentHeadsTopCount: 0,

	logLevel: GgmlLogLevel.GGML_LOG_LEVEL_INFO,
}

export const whisperModelIdToTextLayerCount: { [modelId in WhisperModelId]: number } = {
	'tiny': 4,
	'tiny.en': 4,
	'base': 6,
	'base.en': 6,
	'small': 12,
	'small.en': 12,
	'medium': 24,
	'medium.en': 24,
	'large-v1': 32,
	'large-v2': 32,
	'large-v3': 32,
	'large-v3-turbo': 4,
}

export const whisperModelIdToAlignmentHeadsPreset: { [modelId in WhisperModelId]: WhisperAlignmentHeadsPreset } = {
	'tiny': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_TINY,
	'tiny.en': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_TINY_EN,
	'base': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_BASE,
	'base.en': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_BASE_EN,
	'small': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_SMALL,
	'small.en': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_SMALL_EN,
	'medium': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_MEDIUM,
	'medium.en': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_MEDIUM_EN,
	'large-v1': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_LARGE_V1,
	'large-v2': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_LARGE_V2,
	'large-v3': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_LARGE_V3,
	'large-v3-turbo': WhisperAlignmentHeadsPreset.WHISPER_AHEADS_LARGE_V3_TURBO,
}

export type WhisperModelId =
	'tiny' |
	'tiny.en' |
	'base' |
	'base.en' |
	'small' |
	'small.en' |
	'medium' |
	'medium.en' |
	'large-v1' |
	'large-v2' |
	'large-v3' |
	'large-v3-turbo'

export interface CrossAttentionQKs {
	dimensions: BigInt64Array,
	data: Float32Array
}
