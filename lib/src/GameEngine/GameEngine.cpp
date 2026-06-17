#include <GameEngine/GameEngine.hpp>
#include <GameScenes/Scene.hpp>
#include <SystemManagement/VideoManager.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace YerbEngine {

    GameEngine::GameEngine() {
        /*
         * Set up the paths for the assets and configuration files.
         *
         * The assets directory contains all the images, audio, and font files
         * used in the game. The configuration directory contains the
         * configuration file for the game.
         *
         * The configuration file is a JSON file that contains the game settings
         * such as window size, font path, and more.
         */
        Path const ASSETS_DIR_PATH  = "assets";
        Path const CONFIG_DIR_PATH  = "config";
    Path const ENGINE_CONFIG_FILE_PATH = CONFIG_DIR_PATH / "engine.json";
    Path const DEMO_CONFIG_FILE_PATH   = CONFIG_DIR_PATH / "config.json"; // legacy demo config

        if (!std::filesystem::exists(ASSETS_DIR_PATH)) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Assets folder not found!");
            CleanUp();
            throw std::runtime_error("Assets folder not found!");
        }

        m_configStore = std::make_unique<ConfigStore>(
            std::move(std::make_unique<JsonConfigProvider>(ENGINE_CONFIG_FILE_PATH))
            );
        m_configAdapter = std::make_unique<ConfigAdapter>(*m_configStore);

        // Optionally load demo/game config as a separate named store if present
        if (std::filesystem::exists(DEMO_CONFIG_FILE_PATH)) {
            auto demoStore = std::make_unique<ConfigStore>(
                std::move(std::make_unique<JsonConfigProvider>(DEMO_CONFIG_FILE_PATH)));
            AddConfig("demo", std::move(demoStore));
        }

        m_audioManager = std::make_unique<AudioManager>();

        m_audioSampleQueue = std::make_unique<
            AudioSampleQueue>(*m_audioManager);

        {
            auto const gameCfg = m_configAdapter->getGameConfig();
            m_fontManager = std::make_unique<FontManager>(
                gameCfg.fontPath,
                gameCfg.fontSizeSm,
                gameCfg.fontSizeMd,
                gameCfg.fontSizeLg);
        }

        m_videoManager = std::make_unique<VideoManager>(
            *m_configAdapter);

        m_textureManager = std::make_unique<TextureManager>(
            m_videoManager->getRenderer());

        m_isRunning = true;

        SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                    "Game engine initialized successfully!");
    }

    GameEngine::~GameEngine() { CleanUp(); }


    void GameEngine::CleanUp() {
        SDL_Quit();
        SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                    "Game engine cleaned up successfully!");
    }

    void GameEngine::Update() {
        std::shared_ptr<Scene> const &activeScene =
            m_scenes[m_currentSceneName];
        if (activeScene == nullptr) {
            return;
        }

        activeScene->update();
    }

    bool GameEngine::IsRunning() const { return m_isRunning; }

    void GameEngine::run() {
#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_arg(MainLoop, this, 0, 1);
#else
        while (m_isRunning) {
            MainLoop(this);
        }
#endif
    }

    void GameEngine::quit() {
        SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Quitting game engine...");
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#else
        m_isRunning = false;
#endif
    }

    void GameEngine::LoadScene(std::string const &           sceneName,
                               std::shared_ptr<Scene> const &scene) {
        m_scenes[sceneName] = scene;

        scene->setStartTime(SDL_GetTicks64());
        m_currentSceneName = sceneName;
    }

    void GameEngine::AddConfig(std::string const &name,
                               std::unique_ptr<ConfigStore> store) {
        m_namedAdapters.erase(name);
        m_namedStores[name] = std::move(store);
        m_namedAdapters[name] = std::make_unique<ConfigAdapter>(
            *m_namedStores[name]);
    }

    ConfigAdapter &GameEngine::GetConfig(std::string const &name) const {
        auto it = m_namedAdapters.find(name);
        if (it == m_namedAdapters.end()) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "Config set '%s' not found", name.c_str());
            throw std::runtime_error("Config set not found: " + name);
        }
        return *it->second;
    }

    

    FontManager &GameEngine::getFontManager() const {
        if (!m_fontManager) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "FontManager not initialized");
            throw std::runtime_error("FontManager not initialized");
        }
        return *m_fontManager;
    }

    AudioManager &GameEngine::getAudioManager() const {
        if (!m_audioManager) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "AudioManager not initialized");
            throw std::runtime_error("AudioManager not initialized");
        }
        return *m_audioManager;
    }

    AudioSampleQueue &GameEngine::getAudioSampleQueue() const {
        if (!m_audioSampleQueue) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "AudioSampleManager not initialized");
            throw std::runtime_error("AudioSampleManager not initialized");
        }
        return *m_audioSampleQueue;
    }

    VideoManager &GameEngine::getVideoManager() const {
        if (!m_videoManager) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "VideoManager not initialized");
            throw std::runtime_error("VideoManager not initialized");
        }
        return *m_videoManager;
    }

    TextureManager &GameEngine::getTextureManager() const {
        if (!m_textureManager) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "ImageManager not initialized");
            throw std::runtime_error("ImageManager not initialized");
        }
        return *m_textureManager;
    }

    void GameEngine::S_UserInput() {
        SDL_Event                    event;
        std::shared_ptr<Scene> const activeScene = m_scenes[m_currentSceneName];

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit();
                return;
            }

            // Handle window events
            if (event.type == SDL_WINDOWEVENT) {
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    m_videoManager->updateWindowSize();
                    activeScene->onSceneWindowResize();
                    break;
                default:
                    break;
                }
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                if (!activeScene->getActionMap().contains(
                    event.key.keysym.sym)) {
                    continue;
                }

                ActionState const actionState = event.type == SDL_KEYDOWN
                                                    ? ActionState::START
                                                    : ActionState::END;

                std::string const &actionName =
                    activeScene->getActionMap().at(event.key.keysym.sym);
                Action action(actionName, actionState, std::nullopt);
                activeScene->sDoAction(action);
            }

            if (event.type == SDL_MOUSEBUTTONDOWN ||
                event.type == SDL_MOUSEBUTTONUP) {
                if (!activeScene->getActionMap().contains(
                    event.button.button)) {
                    continue;
                }

                /*
                 * If the event is a mouse button event, set the action state to
                 * start on mouse down, and end on mouse up.
                 */
                ActionState const actionState =
                    event.type == SDL_MOUSEBUTTONDOWN
                        ? ActionState::START
                        : ActionState::END;

                std::string const &actionName =
                    activeScene->getActionMap().at(event.button.button);

                // Get mouse position and convert to game coordinates
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                Vec2 gamePosition{static_cast<float>(mouseX),
                                  static_cast<float>(mouseY)};

                Action action(actionName, actionState, gamePosition);
                activeScene->sDoAction(action);
            }

            // Mouse motion handling
            if (event.type == SDL_MOUSEMOTION &&
                activeScene->getActionMap().contains(SDL_MOUSEMOTION)) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                Vec2 gamePosition{static_cast<float>(mouseX),
                                  static_cast<float>(mouseY)};

                std::string const &actionName =
                    activeScene->getActionMap().at(SDL_MOUSEMOTION);
                Action action(actionName, ActionState::START, gamePosition);
                activeScene->sDoAction(action);
            }
        }
    }

    void GameEngine::MainLoop(void *arg) {
        auto *gameEngine = static_cast<GameEngine *>(arg);
#ifdef __EMSCRIPTEN__
        const bool isWebCanvasEnabled = VideoManager::isWebCanvasEnabled();
        if (!isWebCanvasEnabled) {
            gameEngine->quit();
            return;
        }
#endif
        gameEngine->S_UserInput();
        gameEngine->Update();
    }

} // namespace YerbEngine