# TODO: Decouple engine and demos

The engine now builds on its own (`-DYERB_BUILD_DEMOS=OFF`), and each game ships
its own assets and config through `yerb_add_game`. What is left:

- [ ] Point the tests at `tests/data` (or a minimal test asset bundle) instead of
      `shoot-demo/assets` and the root `config/`. `tests/CMakeLists.txt` still
      copies both, so `-DYERB_BUILD_DEMOS=OFF` leaves the tests reading a demo's
      assets.
- [ ] Move the root `config/` directory under `shoot-demo/`, where the rest of
      that game's data already lives. `pong-demo/config/` is the pattern.
- [ ] Give the web template the game's output name. `template/index.html` loads
      `demo-1.js`, so any second game needs its own page; `configure_file` or a
      build variable would let `yerb_add_game` fill it in.
- [ ] Split the Docker flows, or add a build arg, so one image builds the engine
      and another builds a game.
- [ ] Let `convert-audio.sh` take an assets root. It reads `assets/audio`
      relative to itself, which only works once it is copied into a game.
- [ ] Add `EnginePaths.hpp` and `SdlRuntime.hpp` to `YerbEngine.hpp`. Both demos
      include them by hand because the master header leaves them out.
