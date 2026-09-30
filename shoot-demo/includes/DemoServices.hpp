#pragma once

#include <GameScenes/SceneServices.hpp>

#include "Configuration/DemoConfigAdapter.hpp"

/** The engine's services, plus the one collaborator the demo adds. */
struct DemoServices : YerbEngine::SceneServices {
   DemoConfigAdapter& config;
};
