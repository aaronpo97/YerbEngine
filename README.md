# YerbEngine

YerbEngine is a work-in-progress game engine featuring a hybrid architecture, built with `SDL2` and `C++`. The core engine now lives in `lib/` (public headers in `lib/includes`, implementations in `lib/src`) and powers the playable `shoot-demo` sample game.

## Features

- **Cross-platform** – Supports builds on MacOS, Linux (tested on Fedora), and the Web via Emscripten/WebAssembly.
- **Core library + demo** – Engine code is packaged as a static library (`yerb_engine_core`) and consumed by the `shoot-demo` reference game.
- **Modular ECS design** – Independent systems for input, rendering, and more, enabling scalability.
- **JSON configuration** – Loads game settings using [nlohmann/json](https://github.com/nlohmann/json).
- **Modern C++** – Written in C++23, with code quality enforced by `clang-format` and `clang-tidy`.
- **Asset Management** – Built-in managers for textures, fonts, audio samples, and music tracks.
- **Docker Support** – Containerized build environment for consistent development experience.

## Project Structure

```
lib/                          # Core engine library
├── includes/                 # Public headers (AssetManagement, Configuration, EntityManagement, GameEngine, GameScenes, Helpers, SystemManagement)
└── src/                      # Engine implementations mirroring the include layout
shoot-demo/                   # Playable reference game using the engine
├── includes/                 # Demo-specific scenes and helpers
├── src/                      # Demo gameplay code
└── assets/                   # Audio, fonts, and images bundled into builds
config/                       # JSON configuration consumed at runtime
template/                     # HTML/CSS/JS for Emscripten builds
tests/                        # Boost.Test-based unit tests for engine modules
diagrams/                     # Supporting diagrams (buildable with the Makefile in this folder)
```

## Building from Source

### Prerequisites

**Native builds:**
- CMake 3.21 or higher
- C++23 compatible compiler (Clang, GCC)
- SDL2, SDL2_image, SDL2_ttf, SDL2_mixer

**Web builds:**
- Emscripten SDK

### Native Build (macOS/Linux)

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

Run the demo:
```bash
./shoot-demo   # assets and config are copied into the build directory automatically
```

### Web Build (Emscripten)

```bash
mkdir -p dist-web
cd dist-web
emcmake cmake ..
cmake --build .
```

Serve locally:
```bash
emrun --no_browser --port 3333 bin/index.html
```

Open `http://localhost:3333` in your browser.

### Docker Build

```bash
docker-compose up --build
```

### Formatting
```bash
./format.sh
```
Formats all code in `lib/` and `shoot-demo/` using clang-format.

### Audio Conversion

Convert audio files to the correct formats:
```bash
./convert-audio.sh
```

- Samples → WAV format (for sound effects)
- Tracks → OGG format (for music)

### Tests (native)
After configuring the native build (Boost.Test required):
```bash
cd build
ctest --output-on-failure
```

## Usage

Run `./shoot-demo` from the build directory. On launch, the menu scene appears; follow on-screen instructions to navigate through the demo.

## Contributions

Contributions are welcome! Submit issues or pull requests to help improve the engine.

## License

Licensed under GNU GPL v3.0. See [LICENSE.md](LICENSE.md) for details.
