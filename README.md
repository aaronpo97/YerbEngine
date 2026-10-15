# YerbEngine

YerbEngine is a work-in-progress 2D game engine built with `SDL3` and `C++23`. The core engine lives in `lib/` (public headers in `lib/includes`, implementations in `lib/src`) and powers two playable sample games: `shoot-demo` and `pong-demo`.

## Features

- **Cross-platform:** builds for macOS, Linux (tested on Fedora), and the web via Emscripten/WebAssembly.
- **Core library and games:** the engine is a static library (`yerb_engine_core`); games are declared against it with the `yerb_add_game` CMake helper.
- **Injected dependencies:** a game's `main` assembles the window, the audio, the assets and the clock, then lends them to scenes as a `SceneServices` bundle.
- **Sparse-set ECS:** entities carry a game-defined tag; components live in type-erased `ComponentPool`s behind a `ComponentRegistry`.
- **JSON configuration:** window, font, audio and gameplay values are read through `ConfigStore` and `ConfigReader`, with a typed fallback for every key.
- **Prioritised audio:** `AudioSampleBuffer` queues one-shot samples by priority and cooldown, then spreads them over frames before handing them to the mixer.
- **Asset management:** managers for textures, fonts, audio samples and music tracks.
- **Testable seams:** `IClock`, `IAudioSink`, `IConfigProvider` and `ISceneRouter` let the Boost.Test suite drive engine code with no window and no audio device.
- **Docker support:** containerised Emscripten build and static host.

## Project structure

```
lib/                          # Core engine library (yerb_engine_core)
├── includes/                 # Public headers
│   ├── AssetManagement/      # FontManager, TextureManager, AudioSampleBuffer
│   ├── Configuration/        # ConfigStore, ConfigReader, ConfigAdapter, AudioManifest
│   ├── EntityManagement/     # Entity, EntityManager, ComponentRegistry, Components
│   ├── GameEngine/           # GameEngine, Action, EnginePaths
│   ├── GameScenes/           # Scene, SceneServices, ISceneRouter
│   ├── Helpers/              # Vec2, math, collision, spawn and text helpers
│   ├── Systems/              # RenderSystem
│   └── SystemManagement/     # VideoManager, AudioManager, SdlRuntime, IClock, IAudioSink
└── src/                      # Implementations mirroring the include layout
shoot-demo/                   # Top-down shooter built on the engine
├── includes/                 # Scenes, EntityTags, DemoServices, config adapter
├── src/                      # Gameplay code and main.cpp
└── assets/                   # Audio, fonts and images shipped with the game
pong-demo/                    # Second reference game, laid out like shoot-demo
├── includes/
├── src/
├── assets/
└── config/                   # engine.json, audio.json and pong gameplay config
cmake/                        # Build helpers (YerbAddGame.cmake)
config/                       # shoot-demo's engine.json, audio.json and gameplay config
template/                     # HTML/CSS/JS shell for Emscripten builds
tests/                        # Boost.Test unit tests for engine modules
diagrams/                     # PlantUML architecture diagram (buildable with its Makefile)
```

## Building from source

### Prerequisites

**Native builds:**
- CMake 3.21 or higher
- C++23 compatible compiler (Clang, GCC)
- SDL3, SDL3_image, SDL3_ttf, SDL3_mixer
- Boost.Test, to build the test suite

**Web builds:**
- Emscripten SDK (SDL3_image and SDL3_mixer are fetched and built from source; SDL3 and SDL3_ttf come from Emscripten ports)

`nlohmann/json` is fetched by CMake on every platform.

### Native build (macOS/Linux)

```bash
cmake -S . -B build
cmake --build build
```

Run a game from the build directory. Its assets and config are copied next to the executable after linking:
```bash
cd build
./demo-1   # shoot-demo
./pong     # pong-demo
```

To build the engine and its tests on their own:
```bash
cmake -S . -B build -DYERB_BUILD_DEMOS=OFF
```

### Web build (Emscripten)

```bash
emcmake cmake -S . -B dist-web
cmake --build dist-web
```

Each game is linked as `bin/<output-name>.js` with its assets and config preloaded into the virtual filesystem. The page shell in `template/` is copied alongside it and currently loads `demo-1.js`.

Serve locally:
```bash
emrun --no_browser --port 3333 dist-web/bin/index.html
```

Open `http://localhost:3333` in your browser.

### Docker build

Builds the Emscripten target and serves it on port 3333:
```bash
docker-compose up --build
```

### Formatting
```bash
./format.sh           # format lib/, pong-demo/, shoot-demo/, tests/
./format.sh --check   # report unformatted files without changing them
```

### Tests (native)
```bash
cd build
ctest --output-on-failure
```

### Audio conversion

`convert-audio.sh` uses `ffmpeg` to convert samples to WAV and tracks to OGG. It reads `assets/audio/samples` and `assets/audio/tracks` relative to its own directory, so copy it into the game whose assets you want to convert (`shoot-demo/`, say) and run it from there.

## Architecture

### Composition root

A game's `main` creates every collaborator the engine uses, in dependency order, and hands them out:

```cpp
SdlRuntime const sdl;          // declared first, destroyed last

SdlClock const clock{};
AudioManager mixer{audio.options};
AudioSampleBuffer sampleBuffer{mixer, clock};
VideoManager video{engineConfig};
FontManager fonts{gameConfig.fontPath, FontSizes{/* sm, md, lg */}};
TextureManager textures{RendererHandle{video.getRenderer()}};
GameEngine engine{video, clock};
```

Destruction runs in reverse, which is the order SDL asks for: textures go before the renderer that owns them, and every subsystem is torn down while `SdlRuntime` still holds the library open.

### Scenes and services

A scene is handed a `SceneServices` bundle instead of the engine, so its constructor states what it can touch: the video, the fonts, the textures, the sample buffer, the mixer, the clock and an `ISceneRouter`. A game that adds a collaborator of its own extends the bundle:

```cpp
struct PongServices : YerbEngine::SceneServices {
   PongConfigAdapter& config;
};
```

`Scene` holds the state every game's scenes need: an `EntityManager`, a random generator, the frame delta, and the pause and end flags. Input arrives as a named `Action` and is dispatched to whichever handler is bound to that name, so a scene never compares action strings itself:

```cpp
registerAction(SDLK_W, "LEFT_UP");
registerAction(SDLK_P, "PAUSE");

bindPress("PAUSE", [this](Action&) { m_paused = !m_paused; });
bindHold("LEFT_UP", [this](bool const pressed) {
   setPaddleDirection(m_leftPaddle, Components::CInput::Forward, pressed);
});
```

`bindPress` fires when the action starts, `bindHold` passes true on press and false on release, and `bindAction` sees every state change.

A scene hands control onward by building its successor and naming it. The router takes a constructed scene rather than a type, so a successor can take whatever runtime arguments it needs:

```cpp
m_services.router.LoadScene(
    "Result", std::make_shared<ResultScene>(m_pong, m_mode, m_leftScore, m_rightScore));
```

### Entities and components

`EntityTag` is a `uint32_t`. The engine names only `Default`, `Player` and `Wall`; a game claims its own kinds from `EntityTags::FirstGameTag` upwards:

```cpp
namespace PongTags {
inline constexpr YerbEngine::EntityTag Paddle = YerbEngine::EntityTags::Player;
inline constexpr YerbEngine::EntityTag Ball = YerbEngine::EntityTags::FirstGameTag;
}
```

`EntityManager` owns the entities and a shared `ComponentRegistry`; `ComponentRegistry` keeps one sparse-set `ComponentPool` per component type, created on first use. `RenderSystem::renderEntities` draws every entity carrying a `CShape` and a `CTransform`, as a texture when a `CSprite` is present and a filled rectangle otherwise.

### Configuration

`ConfigStore` flattens a JSON file into dotted keys (`engine.window.size.width`) behind an `IConfigProvider`. `ConfigReader` reads typed values from the store with a fallback per key, so a missing key never fails a launch; `EmptyConfigProvider` holds no keys at all, which lets a game that ships no configuration file use the same code path as one that does. `ConfigAdapter` turns the engine's own keys into a `GameConfig`, and a game wraps a second reader for its gameplay values, as `DemoConfigAdapter` and `PongConfigAdapter` do.

A game's audio is described by a manifest:

```json
{
  "tracks":  [ { "id": "track_play", "file": "audio/tracks/play.ogg" } ],
  "samples": [ { "id": "sample_shoot", "file": "audio/samples/shoot.wav",
                 "volume": 32, "cooldown": 100 } ]
}
```

Paths are relative to the game's assets directory, `volume` is on the 0 to `MAX_VOLUME` scale, and `cooldown` is the milliseconds a sample waits before it may replay.

### Adding a game

Create a directory with `src/` and `includes/`, then declare it:

```cmake
yerb_add_game(
    NAME        my-game
    OUTPUT_NAME my-game
    ASSETS      "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    ASSETS_AT   my-game-assets
    CONFIG      "${CMAKE_CURRENT_SOURCE_DIR}/config"
    CONFIG_AT   my-game-config
)
```

Sources are globbed from `src/`, headers come from `includes/`, and the asset and config directories are copied next to the executable on native builds or preloaded into the virtual filesystem under the same names on Emscripten. `EnginePaths::resolveDirectory` lets `main` prefer the shipped name and fall back to a plain `assets`/`config` during development.

## License

Licensed under GNU GPL v3.0. See [LICENSE.md](LICENSE.md) for details.

## Contributions

Contributions are welcome! Submit issues or pull requests to help improve the engine.
