import { writeFileSync } from 'node:fs'
import { Timer } from './utilities/Timer.js'
import { indexOfMax, yieldToEventLoop } from './utilities/Utilities.js'
import { whisperModelIdToAlignmentHeadsPreset, WhisperCppContext, WhisperModelId } from './WhisperCppContext.js'

async function startTest() {
	const timer = new Timer()

	const context = new WhisperCppContext()

	const modelId: WhisperModelId = 'base.en'

	const alignmentHeadsPreset = whisperModelIdToAlignmentHeadsPreset[modelId]

	let libFilePath: string

	{
		const platform = process.platform
		const arch = process.arch

		if (platform === 'win32' && arch === 'x64') {
			libFilePath = '../whisper.cpp-lib/windows-x64-cpu/whisper.dll'
		} else if (platform === 'win32' && arch === 'arm64') {
			libFilePath = '../whisper.cpp-lib/windows-ar64-cpu/whisper.dll'
		} else if (platform === 'darwin' && arch === 'x64') {
			libFilePath = '../whisper.cpp-lib/macos-universal-cpu-basic/libwhisper.0.dylib'
		} else if (platform === 'darwin' && arch === 'arm64') {
			libFilePath = '../whisper.cpp-lib/macos-universal-cpu-basic/libwhisper.0.dylib'
		} else if (platform === 'linux' && arch === 'x64') {
			libFilePath = '../whisper.cpp-lib/linux-x64-cpu/libwhisper.so.0'
		} else if (platform === 'linux' && arch === 'arm64') {
			libFilePath = '../whisper.cpp-lib/linux-arm64-cpu/libwhisper.so.0'
		} else {
			throw new Error(`Architecture ${platform}-${arch} is not supported by the whisper.cpp binding.`)
		}
	}

	const modelFilePath = `../resources/whisper.cpp-models/ggml-${modelId}.bin`

	await context.initialize({
		libFilePath,
		modelFilePath,

		enableGPU: false,
		alignmentHeadsPreset,
	})

	timer.logAndRestart(`initialize`)

	const vocabSize = context.getVocabSize()

	timer.logAndRestart(`getVocabSize`)

	for (let i = 0; i < 1; i++) {
		{
			const samples = new Float32Array(30 * 16000)

			await context.encodeSamples(samples)

			timer.logAndRestart(`encodeSamples`)
		}

		{
			const melCountPerFrame = modelId.startsWith('large') ? 128 : 80

			const spectrogram = new Float32Array(3000 * melCountPerFrame)

			await context.encodeLogMelSpectrogram(spectrogram, melCountPerFrame)

			timer.logAndRestart(`encodeMelSpectrogram`)

			await yieldToEventLoop()
		}
	}

	const tokens = [50257]

	for (let i = 0; i < 10; i++) {
		timer.restart()
		await context.decodeTokens(tokens, tokens.length - 1)
		timer.logAndRestart(`decodeTokens ${i}`)

		const logits = context.getLogits()
		timer.logAndRestart(`getLogits`)

		const bestToken = indexOfMax(logits)

		tokens.push(bestToken)
	}

	timer.restart()
	const crossAttentionQKs = await context.getCrossAttentionQKs()
	timer.logAndRestart(`getCrossAttentionQKs`)

	const x = 0
}

startTest()
