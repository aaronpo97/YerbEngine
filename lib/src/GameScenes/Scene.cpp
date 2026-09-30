#include <GameScenes/Scene.hpp>
#include <algorithm>

namespace YerbEngine {

void Scene::tick() {
   Uint64 const currentTime = m_services.clock.nowMs();
   m_deltaTime =
       std::min(static_cast<float>(currentTime - m_lastFrameTime) / 1000.0f,
                MAX_FRAME_DELTA);
   m_lastFrameTime = currentTime;
}

void Scene::sAudio() { m_services.audio.update(); }

void Scene::sDoAction(Action& action) {
   auto const handler = m_actionHandlers.find(action.getName());
   if (handler == m_actionHandlers.end()) {
      return;
   }

   handler->second(action);
}

void Scene::bindAction(std::string const& actionName,
                       std::function<void(Action&)> handler) {
   m_actionHandlers[actionName] = std::move(handler);
}

void Scene::bindPress(std::string const& actionName,
                      std::function<void(Action&)> handler) {
   m_actionHandlers[actionName] = [handler =
                                       std::move(handler)](Action& action) {
      if (action.getState() != ActionState::START) {
         return;
      }
      handler(action);
   };
}

void Scene::bindHold(std::string const& actionName,
                     std::function<void(bool)> handler) {
   m_actionHandlers[actionName] = [handler =
                                       std::move(handler)](Action& action) {
      handler(action.getState() == ActionState::START);
   };
}

}  // namespace YerbEngine
