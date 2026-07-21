import NodePath from 'path'

export class SharedLibraryPathUpdater {
	readonly paths: string[]

	private readonly envVarName: string
	private originalEnvVarValue?: string

	constructor(pathsToAdd: string[]) {
		this.paths = pathsToAdd

		switch (process.platform) {
			case 'win32':
				this.envVarName = 'PATH'
				break
			case 'linux':
				this.envVarName = 'LD_LIBRARY_PATH'
				break
			case 'darwin':
				this.envVarName = 'DYLD_LIBRARY_PATH'
				// Note for macOS: DYLD_LIBRARY_PATH can be ignored in certain secure contexts
				// (e.g., sandboxed apps, setuid binaries, or when System Integrity Protection (SIP) is active).
				// For typical Node.js CLI apps, it usually works. For robust macOS solutions,
				// `rpath` (a compile-time setting) is generally preferred.
				break
			default:
				throw new Error(`Unsupported platform for dynamic library path management: ${process.platform}`)
		}
	}

	apply(mode: 'prepend' | 'append' = 'prepend' ) {
		if (this.originalEnvVarValue) {
			return
		}

		if (this.paths.length === 0) {
			return
		}

		this.originalEnvVarValue = process.env[this.envVarName]

		const paths = this.paths.map(path => NodePath.resolve(path))
		const pathString = paths.join(NodePath.delimiter)

		if (mode === 'prepend') {
			process.env[this.envVarName] = `${pathString}${NodePath.delimiter}${this.originalEnvVarValue}`
		} else if (mode === 'append') {
			process.env[this.envVarName] = `${this.originalEnvVarValue}${NodePath.delimiter}${pathString}`
		} else {
			throw new Error(`Invalid position argument`)
		}
	}

	restore() {
		if (!this.originalEnvVarValue) {
			return
		}

		process.env[this.envVarName] = this.originalEnvVarValue

		this.originalEnvVarValue = undefined
	}
}
