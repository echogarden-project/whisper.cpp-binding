export function roundToDigits(val: number, digits = 3) {
	const multiplier = 10 ** digits

	return Math.round(val * multiplier) / multiplier
}

export function writeToStderr(message: any) {
	process.stderr.write(message)
}

export function logToStderr(message: any) {
	writeToStderr(message)
	writeToStderr('\n')
}

export function indexOfMax(vector: ArrayLike<number>) {
	if (vector.length == 0) {
		return -1
	}

	let maxValue = vector[0]
	let result = 0

	for (let i = 1; i < vector.length; i++) {
		if (vector[i] > maxValue) {
			maxValue = vector[i]
			result = i
		}
	}

	return result
}

export function yieldToEventLoop() {
	return new Promise((resolve) => {
		setImmediate(resolve)
	})
}
