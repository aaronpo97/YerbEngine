#pragma once
#include <GameEngine/Action.hpp>
#include <GameEngine/GameEngine.hpp>
#include <GameScenes/ActionQueue.hpp>
#include <map>
#include <string>
#include <utility>

namespace YerbEngine {

    typedef std::map<int, std::string> ActionMap;

    class Scene {
      protected:
        GameEngine *m_gameEngine;
        Uint64      m_lastFrameTime  = 0;
        float       m_deltaTime      = 0;
        bool        m_endTriggered   = false;
        bool        m_hasEnded       = false;
        bool        m_paused         = false;
        Uint64      m_SceneStartTime = 0;
        ActionMap   m_actionMap;
        ActionQueue m_actions;

      public:
        explicit Scene(GameEngine *gameEngine) : m_gameEngine(gameEngine) {}

        virtual ~Scene()                       = default;
        virtual void update()                  = 0;
        virtual void onEnd()                   = 0;
        virtual void sRender()                 = 0;
        virtual void sDoAction(Action &action) = 0;
        virtual void sAudio()                  = 0;

        virtual void onSceneWindowResize() = 0;

        void enqueueAction(Action action) {
            m_actions.push(std::move(action));
        }

        void registerAction(int const          inputKey,
                            std::string const &actionName) {
            m_actionMap[inputKey] = actionName;
        }
        void             setPaused(bool const paused) { m_paused = paused; }
        ActionMap const &getActionMap() const { return m_actionMap; }
        void             setStartTime(Uint64 const startTime) {
            m_SceneStartTime = startTime;
        }
        Uint64 const &getStartTime() const { return m_SceneStartTime; }

      protected:
        void processQueuedActions() {
            if (m_actions.empty()) {
                return;
            }

            auto actions = m_actions.drain();
            for (auto &action : actions) {
                sDoAction(action);
            }
        }
    };

} // namespace YerbEngine
