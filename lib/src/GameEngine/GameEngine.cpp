#include <GameEngine/GameEngine.hpp>
#include <GameScenes/Scene.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace YerbEngine {

GameEngine::GameEngine(VideoManager& video, IClock const& clock)
    : m_video(video), m_clock(clock) {
   SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
               "Game engine initialized successfully!");
}

std::shared_ptr<Scene> GameEngine::findActiveScene() const {
   auto const scene = m_scenes.find(m_currentSceneName);
   return scene == m_scenes.end() ? nullptr : scene->second;
}

void GameEngine::Update() {
   std::shared_ptr<Scene> const activeScene = findActiveScene();
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

void GameEngine::LoadScene(std::string const& sceneName,
                           std::shared_ptr<Scene> const& scene) {
   /*
    * A route always hands over a freshly built scene, so the one being
    * left is released here rather than held, with its entities, for the
    * rest of the run. The frame loop holds its own reference, which is
    * what lets a scene route from inside its own update().
    */
   std::erase_if(m_scenes, [&sceneName](auto const& entry) {
      return entry.first != sceneName;
   });

   m_scenes[sceneName] = scene;

   scene->setStartTime(m_clock.nowMs());
   m_currentSceneName = sceneName;
}

void GameEngine::S_UserInput() {
   SDL_Event event;
   std::shared_ptr<Scene> const activeScene = findActiveScene();

   while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
         quit();
         return;
      }

      // The queue is still drained without a scene, so that quitting works.
      if (activeScene == nullptr) {
         continue;
      }

      // Handle window events
      if (event.type == SDL_EVENT_WINDOW_RESIZED ||
          event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
         m_video.updateWindowSize();
         activeScene->onSceneWindowResize();
      }

      if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
         if (!activeScene->getActionMap().contains(event.key.key)) {
            continue;
         }

         ActionState const actionState = event.type == SDL_EVENT_KEY_DOWN
                                             ? ActionState::START
                                             : ActionState::END;

         std::string const& actionName =
             activeScene->getActionMap().at(event.key.key);
         Action action(actionName, actionState, std::nullopt);
         activeScene->sDoAction(action);
      }

      if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
          event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
         if (!activeScene->getActionMap().contains(event.button.button)) {
            continue;
         }

         /*
          * If the event is a mouse button event, set the action state to
          * start on mouse down, and end on mouse up.
          */
         ActionState const actionState =
             event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? ActionState::START
                                                       : ActionState::END;

         std::string const& actionName =
             activeScene->getActionMap().at(event.button.button);

         // Get mouse position and convert to game coordinates
         float mouseX, mouseY;
         SDL_GetMouseState(&mouseX, &mouseY);

         Vec2 gamePosition{mouseX, mouseY};

         Action action(actionName, actionState, gamePosition);
         activeScene->sDoAction(action);
      }

      // Mouse motion handling
      if (event.type == SDL_EVENT_MOUSE_MOTION &&
          activeScene->getActionMap().contains(SDL_EVENT_MOUSE_MOTION)) {
         float mouseX, mouseY;
         SDL_GetMouseState(&mouseX, &mouseY);
         Vec2 gamePosition{mouseX, mouseY};

         std::string const& actionName =
             activeScene->getActionMap().at(SDL_EVENT_MOUSE_MOTION);
         Action action(actionName, ActionState::START, gamePosition);
         activeScene->sDoAction(action);
      }
   }
}

void GameEngine::MainLoop(void* arg) {
   auto* gameEngine = static_cast<GameEngine*>(arg);
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

}  // namespace YerbEngine
