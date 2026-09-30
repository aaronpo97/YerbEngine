#pragma once

#include <GameScenes/Scene.hpp>

#include "DemoServices.hpp"

/**
 * Base for the shooter demo's scenes.
 *
 * Holds the demo-shaped view of the bundle, which a scene needs both to
 * read the game's configuration and to build its successor.
 */
class DemoScene : public YerbEngine::Scene {
  protected:
   DemoServices const& m_demo;

   explicit DemoScene(DemoServices const& services)
       : YerbEngine::Scene(services), m_demo(services) {}
};
