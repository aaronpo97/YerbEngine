# TODO: Decouple Engine and Demo

- [ ] Add a CMake option (e.g., `BUILD_DEMO`) to control `add_subdirectory(shoot-demo)` so engine builds can exclude the demo.
- [ ] Move demo asset/config copy steps into the demo target only; remove engine-wide targets that assume `shoot-demo/assets`.
- [ ] Update tests to use dedicated test assets under `tests/data` (or a minimal test asset bundle) instead of `shoot-demo/assets`.
- [ ] Parameterize the web template output name (avoid hardcoded `demo-1.js`) using `configure_file` or a build variable.
- [ ] Split Docker flows: one for engine-only builds, one for demo (or add a build arg to select targets).
- [ ] Make `convert-audio.sh` accept an assets root or relocate it under `shoot-demo/` to avoid implying engine ownership.
- [ ] Refresh README to document optional demo build and the new asset/test/tooling locations.
