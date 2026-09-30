#pragma once

#include <GameScenes/SceneServices.hpp>

#include "Configuration/PongConfigAdapter.hpp"

/** The engine's services, plus the one collaborator pong adds of its own. */
struct PongServices : YerbEngine::SceneServices {
   PongConfigAdapter& config;
};
