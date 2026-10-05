#pragma once

#include <SystemManagement/IClock.hpp>
#include <SystemManagement/VideoManager.hpp>
#include <map>
#include <memory>
#include <string>

#include "GameScenes/ISceneRouter.hpp"

namespace YerbEngine {

class Scene;

/**
 * Runs the frame loop and owns the scenes.
 *
 * The engine is handed the collaborators it needs rather than building
 * them: the composition root (your main) assembles the window, the audio
 * and the assets, and lends scenes the same instances through a
 * SceneServices bundle.
 */
class GameEngine : public ISceneRouter {
  protected:
   VideoManager& m_video;
   IClock const& m_clock;

   std::map<std::string, std::shared_ptr<Scene>> m_scenes;
   std::string m_currentSceneName;
   bool m_isRunning = true;

   /**
    * The scene registered under the current name, or nullptr when there
    * is none.
    *
    * Returned by value: a scene may route to its successor mid-frame,
    * which drops the engine's reference to the scene still being driven.
    */
   std::shared_ptr<Scene> findActiveScene() const;

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
   static void MainLoop(void* arg);

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
    * @param video The window and renderer the active scene draws into.
    * @param clock The timeline scene start times are stamped from.
    */
   GameEngine(VideoManager& video, IClock const& clock);

   ~GameEngine() override = default;

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
   void LoadScene(std::string const& sceneName,
                  std::shared_ptr<Scene> const& scene) override;

   /**
    * Called by the entry point to your application.
    */
   void run();

   /**
    * Quits the game engine by either stopping the main loop or canceling
    * the emscripten main loop.
    */
   void quit() override;
};

}  // namespace YerbEngine
