#pragma once
#include <SDL3/SDL.h>

#include <EntityManagement/EntityManager.hpp>
#include <GameEngine/Action.hpp>
#include <GameEngine/GameEngine.hpp>
#include <map>
#include <random>
#include <string>

namespace YerbEngine {

typedef std::map<int, std::string> ActionMap;

class Scene {
  protected:
   GameEngine* m_gameEngine;

   EntityManager m_entities;
   std::mt19937 m_randomGenerator{std::random_device{}()};
   Uint64 m_lastFrameTime = 0;
   float m_deltaTime = 0;
   bool m_endTriggered = false;
   bool m_hasEnded = false;
   bool m_paused = false;
   Uint64 m_SceneStartTime = 0;
   ActionMap m_actionMap;

   /**
    * Refreshes m_deltaTime from the time elapsed since the previous call.
    *
    * Call once at the top of update(). The delta is capped at
    * MAX_FRAME_DELTA so a long stall (a breakpoint, a dragged window, a
    * backgrounded tab) cannot teleport entities through each other.
    */
   void tick();

  public:
   static constexpr float MAX_FRAME_DELTA = 0.05f;

   explicit Scene(GameEngine* gameEngine)
       : m_gameEngine(gameEngine), m_lastFrameTime(SDL_GetTicks()) {}

   virtual ~Scene() = default;
   virtual void update() = 0;
   virtual void onEnd() = 0;
   virtual void sRender() = 0;
   virtual void sDoAction(Action& action) = 0;

   /**
    * Advances queued audio. Override to drive music as well, calling
    * Scene::sAudio() for the sample buffer.
    */
   virtual void sAudio();

   virtual void onSceneWindowResize() {}

   void registerAction(int const inputKey, std::string const& actionName) {
      m_actionMap[inputKey] = actionName;
   }
   void setPaused(bool const paused) { m_paused = paused; }
   ActionMap const& getActionMap() const { return m_actionMap; }
   void setStartTime(Uint64 const startTime) { m_SceneStartTime = startTime; }
   Uint64 const& getStartTime() const { return m_SceneStartTime; }
};

}  // namespace YerbEngine
