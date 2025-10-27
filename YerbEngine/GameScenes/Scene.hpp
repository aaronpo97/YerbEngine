#pragma once
#include <YerbEngine/GameEngine/Action.hpp>
#include <YerbEngine/GameEngine/GameEngine.hpp>
#include <map>
#include <string>

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

  public:
    explicit Scene(GameEngine *gameEngine) : m_gameEngine(gameEngine) {};

    virtual ~Scene()                       = default;
    virtual void update()                  = 0;
    virtual void onEnd()                   = 0;
    virtual void sRender()                 = 0;
    virtual void sDoAction(Action &action) = 0;
    virtual void sAudio()                  = 0;

    virtual void onSceneWindowResize() = 0;

    void registerAction(int const inputKey, std::string const &actionName) {
        m_actionMap[inputKey] = actionName;
    }
    void             setPaused(bool const paused) { m_paused = paused; }
    ActionMap const &getActionMap() const { return m_actionMap; }
    void setStartTime(Uint64 const startTime) { m_SceneStartTime = startTime; }
    Uint64 const &getStartTime() const { return m_SceneStartTime; }
};
