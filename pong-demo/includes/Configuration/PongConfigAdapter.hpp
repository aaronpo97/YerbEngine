#pragma once
#include <SDL3/SDL.h>

#include <Configuration/ConfigAdapter.hpp>
#include <Configuration/ConfigReader.hpp>
#include <Configuration/ConfigStore.hpp>
#include <string>

#include "PongConfigTypes.hpp"

// Pong-specific adapter that sources window/fonts from the engine adapter
// and gameplay configs from the pong config store.
class PongConfigAdapter {
   YerbEngine::ConfigAdapter& m_engine;
   YerbEngine::ConfigReader m_pong;

   ShapeConfig shapeOr(std::string const& base,
                       ShapeConfig const& fallback) const {
      return ShapeConfig{m_pong.floatOr(base + ".height", fallback.height),
                         m_pong.floatOr(base + ".width", fallback.width),
                         m_pong.colorOr(base + ".color", fallback.color)};
   }

  public:
   PongConfigAdapter(YerbEngine::ConfigAdapter& engine,
                     YerbEngine::ConfigStore& pongStore)
       : m_engine(engine), m_pong(pongStore) {}

   YerbEngine::GameConfig getGameConfig() const {
      return m_engine.getGameConfig();
   }

   MatchConfig getMatchConfig() const {
      return MatchConfig{
          .pointsToWin = m_pong.intOr("matchConfig.pointsToWin", 7),
          .serveDelay = m_pong.u64Or("matchConfig.serveDelay", 1200),
      };
   }

   PaddleConfig getPaddleConfig() const {
      constexpr SDL_Color defaultColor{235, 235, 235, 255};
      return PaddleConfig{
          .speed = m_pong.floatOr("paddleConfig.speed", 9.f),
          .wallOffset = m_pong.floatOr("paddleConfig.wallOffset", 48.f),
          .shape =
              shapeOr("paddleConfig.shape", ShapeConfig{130, 18, defaultColor}),
      };
   }

   BallConfig getBallConfig() const {
      constexpr SDL_Color defaultColor{255, 255, 255, 255};
      return BallConfig{
          .baseSpeed = m_pong.floatOr("ballConfig.baseSpeed", 7.5f),
          .speedIncrement = m_pong.floatOr("ballConfig.speedIncrement", 0.4f),
          .maxSpeed = m_pong.floatOr("ballConfig.maxSpeed", 16.f),
          .maxBounceAngle =
              m_pong.floatOr("ballConfig.maxBounceAngle", 1.0472f),
          .shape =
              shapeOr("ballConfig.shape", ShapeConfig{18, 18, defaultColor}),
      };
   }

   OpponentConfig getOpponentConfig() const {
      return OpponentConfig{
          .speed = m_pong.floatOr("opponentConfig.speed", 7.f),
          .deadZone = m_pong.floatOr("opponentConfig.deadZone", 14.f),
          .reactionDistance =
              m_pong.floatOr("opponentConfig.reactionDistance", 0.62f),
      };
   }
};
