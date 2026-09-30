#pragma once

#include <GameScenes/Scene.hpp>

#include "PongServices.hpp"

/**
 * Base for pong's scenes.
 *
 * Holds the pong-shaped view of the bundle, which a scene needs both to
 * read the game's configuration and to build its successor.
 */
class PongScene : public YerbEngine::Scene {
  protected:
   PongServices const& m_pong;

   explicit PongScene(PongServices const& services)
       : YerbEngine::Scene(services), m_pong(services) {}
};
