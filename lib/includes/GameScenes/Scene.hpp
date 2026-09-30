#pragma once
#include <SDL3/SDL.h>

#include <EntityManagement/EntityManager.hpp>
#include <GameEngine/Action.hpp>
#include <GameScenes/SceneServices.hpp>
#include <functional>
#include <map>
#include <random>
#include <string>
#include <unordered_map>

namespace YerbEngine {

typedef std::map<int, std::string> ActionMap;

class Scene {
  protected:
   /** The engine collaborators this scene was lent. */
   SceneServices const& m_services;

   EntityManager m_entities;
   std::mt19937 m_randomGenerator{std::random_device{}()};
   Uint64 m_lastFrameTime = 0;
   float m_deltaTime = 0;
   bool m_endTriggered = false;
   bool m_hasEnded = false;
   bool m_paused = false;
   Uint64 m_SceneStartTime = 0;
   ActionMap m_actionMap;
   std::unordered_map<std::string, std::function<void(Action&)>>
       m_actionHandlers;

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

   explicit Scene(SceneServices const& services)
       : m_services(services), m_lastFrameTime(services.clock.nowMs()) {}

   virtual ~Scene() = default;
   virtual void update() = 0;
   virtual void onEnd() = 0;
   virtual void sRender() = 0;

   /**
    * Routes an action to the handler bound to its name.
    *
    * Unbound actions are ignored. Override only to wrap the dispatch
    * (a guard, logging); call Scene::sDoAction() from the override.
    */
   virtual void sDoAction(Action& action);

   /**
    * Advances queued audio. Override to drive music as well, calling
    * Scene::sAudio() for the sample buffer.
    */
   virtual void sAudio();

   virtual void onSceneWindowResize() {}

   /**
    * Maps an input key (or mouse button) onto an action name. Several keys
    * may share one action name.
    */
   void registerAction(int const inputKey, std::string const& actionName) {
      m_actionMap[inputKey] = actionName;
   }

   /** Runs the handler for every state change of the action. */
   void bindAction(std::string const& actionName,
                   std::function<void(Action&)> handler);

   /** Runs the handler when the action begins, not when it ends. */
   void bindPress(std::string const& actionName,
                  std::function<void(Action&)> handler);

   /**
    * Runs the handler with true when the action begins and false when it
    * ends, for inputs held down over several frames.
    */
   void bindHold(std::string const& actionName,
                 std::function<void(bool)> handler);
   void setPaused(bool const paused) { m_paused = paused; }
   ActionMap const& getActionMap() const { return m_actionMap; }
   void setStartTime(Uint64 const startTime) { m_SceneStartTime = startTime; }
   Uint64 const& getStartTime() const { return m_SceneStartTime; }
};

}  // namespace YerbEngine
