# YerbEngine

YerbEngine is a work-in-progress game engine featuring a hybrid architecture, built with `SDL3` and `C++`. The core engine lives in `lib/` (public headers in `lib/includes`, implementations in `lib/src`) and powers two playable sample games: `shoot-demo` and `pong-demo`.

## Features

- **Cross-platform** – Supports builds on MacOS, Linux (tested on Fedora), and the Web via Emscripten/WebAssembly.
- **Core library + games** – Engine code is packaged as a static library (`yerb_engine_core`) and consumed by the reference games through the `yerb_add_game` CMake helper.
- **Modular ECS design** – Independent systems for input, rendering, and more, enabling scalability.
- **JSON configuration** – Loads game settings, and each game's audio manifest, using [nlohmann/json](https://github.com/nlohmann/json).
- **Modern C++** – Written in C++23, with code quality enforced by `clang-format` and `clang-tidy`.
- **Asset Management** – Built-in managers for textures, fonts, audio samples, and music tracks.
- **Docker Support** – Containerized build environment for consistent development experience.

## Project Structure

```
lib/                          # Core engine library
├── includes/                 # Public headers (AssetManagement, Configuration, EntityManagement, GameEngine, GameScenes, Helpers, SystemManagement)
└── src/                      # Engine implementations mirroring the include layout
shoot-demo/                   # Playable reference game using the engine
├── includes/                 # Game-specific scenes and helpers
├── src/                      # Gameplay code
└── assets/                   # Audio, fonts, and images bundled into builds
pong-demo/                    # Second reference game, laid out like shoot-demo
├── includes/
├── src/
├── assets/
└── config/                   # Pong gameplay configuration
cmake/                        # Build helpers (yerb_add_game)
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
- SDL3, SDL3_image, SDL3_ttf, SDL3_mixer

**Web builds:**
- Emscripten SDK

### Native Build (macOS/Linux)

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

Run a game (assets and config are copied into the build directory automatically):
```bash
./demo-1   # shoot-demo
./pong     # pong-demo
```

To build the engine and its tests on their own:
```bash
cmake .. -DYERB_BUILD_DEMOS=OFF
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
./format.sh           # format lib/, pong-demo/, shoot-demo/, tests/
./format.sh --check   # report unformatted files without changing them
```

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

Run `./demo-1` or `./pong` from the build directory. On launch, the menu scene appears; follow on-screen instructions to navigate through the game.

## Contributions

Contributions are welcome! Submit issues or pull requests to help improve the engine.

## License

Licensed under GNU GPL v3.0. See [LICENSE.md](LICENSE.md) for details.
