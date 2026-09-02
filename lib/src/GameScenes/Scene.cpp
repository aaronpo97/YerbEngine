#include <GameScenes/Scene.hpp>
#include <algorithm>

namespace YerbEngine {

void Scene::tick() {
   Uint64 const currentTime = SDL_GetTicks();
   m_deltaTime =
       std::min(static_cast<float>(currentTime - m_lastFrameTime) / 1000.0f,
                MAX_FRAME_DELTA);
   m_lastFrameTime = currentTime;
}

void Scene::sAudio() { m_gameEngine->getAudioSampleBuffer().update(); }

}  // namespace YerbEngine
