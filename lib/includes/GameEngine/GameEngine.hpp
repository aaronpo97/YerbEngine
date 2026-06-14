#pragma once

#include "Configuration/ConfigStore.hpp"

#include <AssetManagement/AudioSampleQueue.hpp>
#include <AssetManagement/FontManager.hpp>
#include <AssetManagement/TextureManager.hpp>
#include <Configuration/ConfigManagerDeprecated.hpp>
#include <SystemManagement/AudioManager.hpp>
#include <SystemManagement/VideoManager.hpp>

#include <filesystem>
#include <map>
#include <string>

namespace YerbEngine {

    typedef std::filesystem::path Path;
    class Scene;

    class GameEngine {
    protected:
        std::map<std::string, std::shared_ptr<Scene>> m_scenes;
        std::string                                   m_currentSceneName;
        bool                                          m_isRunning = false;

        std::unique_ptr<ConfigManagerDeprecated>                m_configManager_deprecated;
        std::unique_ptr<FontManager>                  m_fontManager;
        std::unique_ptr<AudioManager>                 m_audioManager;
        std::unique_ptr<TextureManager>               m_textureManager;
        std::unique_ptr<AudioSampleQueue>             m_audioSampleQueue;
        std::unique_ptr<VideoManager>                 m_videoManager;
        std::unique_ptr<ConfigStore>                  m_configStore;


        /**
         * Calls the active scene's update method.
         *
         * This is used in the main loop to update on each frame.
         */
        void Update();

        /**
         * The main loop function that is called by the game engine.
         *
         * This function is responsible for calling the user input system and
         * updating the game engine.
         *
         * @param arg A pointer to the GameEngine object
         */
        static void MainLoop(void *arg);

        /**
         * Calls SDL_Quit to release all SDL resources.
         *
         * Called when the game engine is destructed.
         */
        static void CleanUp();

        /**
         * This is the method used by the game engine to manage the user input
         * system.
         *
         * The game engine listens for SDL events such as key presses, mouse
         * clicks, and window events.
         *
         * If the current active scene uses the event as a Scene Action, the
         * action is triggered.
         *
         */
        void S_UserInput();

    public:
        /**
         * Constructs the GameEngine object and initializes all necessary
         * managers and resources.
         *
         * @throws std::runtime_error if the assets directory is not found.
         */
        GameEngine();

        /**
         * Destroys the GameEngine object and cleans up all resources.
         *
         * Automatic cleanup for when the game engine goes out of scope.
         */
        ~GameEngine();

        /**
         * Checks if the game engine is running.
         *
         * @returns true if the game engine is running, false otherwise.
         */
        bool IsRunning() const;

        /**
         * Loads a scene into the game engine.
         *
         * @param sceneName The name of the scene to load
         * @param scene A shared pointer to the scene object to load
         */
        void LoadScene(std::string const &           sceneName,
                       std::shared_ptr<Scene> const &scene);

        /**
         * Retrieves the ConfigManager instance associated with the game engine.
         *
         * @throws std::runtime_error if ConfigManager is not initialized.
         * @returns A reference to the initialized ConfigManager object.
         */
        ConfigManagerDeprecated &GetConfigManager() const;

        /**
         * Retrieves the FontManager instance associated with the game engine.
         *
         * @throws std::runtime_error if FontManager is not initialized.
         * @returns A reference to the initialized FontManager object.
         */
        FontManager &getFontManager() const;

        /**
         * Retrieves the AudioManager instance associated with the game engine.
         *
         * @throws std::runtime_error if AudioManager is not initialized.
         * @returns A reference to the initialized AudioManager object.
         */
        AudioManager &getAudioManager() const;

        /**
         * Retrieves the AudioSampleQueue instance associated with the game
         * engine.
         *
         * @throws std::runtime_error if AudioSampleQueue is not initialized.
         * @returns A reference to the initialized AudioSampleQueue object.
         */
        AudioSampleQueue &getAudioSampleQueue() const;

        /**
         * Retrieves the VideoManager instance associated with the game engine.
         *
         * @throws std::runtime_error if VideoManager is not initialized.
         * @returns A reference to the initialized VideoManager object.
         */
        VideoManager &getVideoManager() const;

        /**
         * Retrieves the TextureManager instance associated with the game
         * engine.
         *
         * @throws std::runtime_error if TextureManager is not initialized.
         * @returns A reference to the initialized TextureManager object.
         */
        TextureManager &getTextureManager() const;

        /**
         * Called by the entry point to your application.
         */
        void run();

        /**
         * Quits the game engine by either stopping the main loop or canceling
         * the emscripten main loop.
         */
        void quit();
    };

} // namespace YerbEngine