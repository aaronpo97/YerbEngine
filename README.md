# YerbEngine

YerbEngine is a work-in-progress game engine featuring an _Entity-Component-System_ (ECS) architecture, built with `SDL2` and modern `C++`.

## Features

- **Cross-platform** – Supports building on macOS, Linux (Fedora), and the Web via Emscripten/WebAssembly.
- **Modular ECS design** – Independent systems for input, rendering, and more, enabling scalability.
- **JSON configuration** – Loads game settings using [nlohmann/json](https://github.com/nlohmann/json).
- **Modern C++** – Written in C++23, with code quality enforced by `clang-tidy`.

## Installation

Currently, only source builds are available.

### Building from Source

#### Emscripten (Recommended)

To build for the web, run:

```bash
mkdir dist-web
cd dist-web
emcmake cmake ..
cmake --build .
```

A web template is provided at `/template` and serves as the shell for the project.

##### Local Web Server

After building, serve the app locally with Emscripten’s HTTP server:

```bash
cd dist-web/build
emrun --no_browser --port 3333 .
```

#### macOS/Linux

To build natively:

```bash
mkdir dist
cd dist
cmake ..
make 2>&1 | tee build.log
```

##### Running the Application

```bash
cd build
./SDL_GAME
```

## Usage

On launch, the menu scene appears. Follow on-screen instructions at the bottom of the window.

## Contributions

Contributions are welcome! Submit issues or pull requests to help improve the engine.

## License

Licensed under GNU GPL v3.0. See [LICENSE.md](LICENSE) for details.
