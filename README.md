# YerbEngine

YerbEngine is a work-in-progress game engine featuring a hybrid architecture, built with `SDL2` and `C++`.
I am currently in the process of refactoring this engine to be a entity component system.

## Features

- **Cross-platform** – Supports builds on MacOS, Linux (tested on Fedora), and the Web via Emscripten/WebAssembly.
- **Modular ECS design** – Independent systems for input, rendering, and more, enabling scalability.
- **JSON configuration** – Loads game settings using [nlohmann/json](https://github.com/nlohmann/json).
- **Modern C++** – Written in C++23, with code quality enforced by `clang-format` and `clang-tidy`.
- **Asset Management** – Built-in managers for textures, fonts, audio samples, and music tracks.
- **Docker Support** – Containerized build environment for consistent development experience.

## Project Structure

```
YerbEngine/                   # Core engine headers
├── AssetManagement/          # Texture, font, and audio managers
├── Configuration/            # Config structures and JSON parsing
├── EntityManagement/         # ECS components, entities, entity manager
├── GameEngine/               # Main engine class and action system
├── GameScenes/               # Scene base class
├── Helpers/                  # Utility namespaces (collision, movement, spawn, etc.)
└── SystemManagement/         # Audio and video managers

src/                          # Core engine implementation (.cpp files)
shoot-demo/                   # Shoot demo game implementation
config/                       # JSON configuration files
assets/                       # Game assets (audio, fonts, images)
template/                     # Web build HTML templates
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
mkdir build
cd build
cmake ..
make 2>&1 | tee build.log
```

Run the demo:
```bash
./demo-1
```

### Web Build (Emscripten)

```bash
mkdir dist-web
cd dist-web
emcmake cmake ..
cmake --build .
```

Serve locally:
```bash
emrun --no_browser --port 3333 bin/
```

Open `http://localhost:3333` in your browser.

### Docker Build

```bash
docker-compose up --build
```

#### macOS/Linux

To build natively:

Run the formatter before committing:
```bash
./format.sh
```

This formats all code in `YerbEngine/`, `src/`, and `shoot-demo/` using clang-format.

### Audio Conversion

Convert audio files to the correct formats:
```bash
./convert-audio.sh
```

- Samples → WAV format (for sound effects)
- Tracks → OGG format (for music)

## Usage

On launch, the menu scene appears. Follow on-screen instructions to navigate through the demo.

## TODO

- [] Decouple configuration, sprite management, movement helpers from core engine code.

## Contributions

Contributions are welcome! Submit issues or pull requests to help improve the engine.

## License

Licensed under GNU GPL v3.0. See [LICENSE.md](LICENSE) for details.
