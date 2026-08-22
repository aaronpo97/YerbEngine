#pragma once
#include <Configuration/ConfigAdapter.hpp>
#include <Configuration/ConfigStore.hpp>
#include <SDL.h>
#include <string>

#include "PongConfigTypes.hpp"

class PongConfigAdapter {
    YerbEngine::ConfigAdapter &m_engine;
    YerbEngine::ConfigStore   &m_demoStore;

    static inline ShapeConfig shapeOr(ShapeConfig              def,
                                      YerbEngine::ConfigStore &store,
                                      std::string const       &base) {
        float     w = YerbEngine::ConfigAdapter::floatOr(def.width, store,
                                                         base + ".width");
        float     h = YerbEngine::ConfigAdapter::floatOr(def.height, store,
                                                         base + ".height");
        SDL_Color c = YerbEngine::ConfigAdapter::colorOr(def.color, store,
                                                         base + ".color");
        return ShapeConfig{h, w, c};
    }

  public:
    PongConfigAdapter(YerbEngine::ConfigAdapter &engine,
                      YerbEngine::ConfigStore   &demoStore)
        : m_engine(engine),
          m_demoStore(demoStore) {}

    YerbEngine::GameConfig getGameConfig() { return m_engine.getGameConfig(); }

    MatchConfig getMatchConfig() {
        MatchConfig cfg{};
        cfg.pointsToWin = YerbEngine::ConfigAdapter::intOr(
            7, m_demoStore, "matchConfig.pointsToWin");
        cfg.serveDelay = YerbEngine::ConfigAdapter::u64Or(
            1200, m_demoStore, "matchConfig.serveDelay");
        return cfg;
    }

    PaddleConfig getPaddleConfig() {
        constexpr SDL_Color defaultColor{235, 235, 235, 255};
        PaddleConfig        cfg{
            .speed = YerbEngine::ConfigAdapter::floatOr(9.f, m_demoStore,
                                                        "paddleConfig.speed"),
            .wallOffset = YerbEngine::ConfigAdapter::floatOr(
                48.f, m_demoStore, "paddleConfig.wallOffset"),
            .shape = shapeOr(ShapeConfig{130, 18, defaultColor}, m_demoStore,
                             "paddleConfig.shape"),
        };
        return cfg;
    }

    BallConfig getBallConfig() {
        constexpr SDL_Color defaultColor{255, 255, 255, 255};
        BallConfig          cfg{
            .baseSpeed = YerbEngine::ConfigAdapter::floatOr(
                7.5f, m_demoStore, "ballConfig.baseSpeed"),
            .speedIncrement = YerbEngine::ConfigAdapter::floatOr(
                0.4f, m_demoStore, "ballConfig.speedIncrement"),
            .maxSpeed = YerbEngine::ConfigAdapter::floatOr(
                16.f, m_demoStore, "ballConfig.maxSpeed"),
            .maxBounceAngle = YerbEngine::ConfigAdapter::floatOr(
                1.0472f, m_demoStore, "ballConfig.maxBounceAngle"),
            .shape = shapeOr(ShapeConfig{18, 18, defaultColor}, m_demoStore,
                             "ballConfig.shape"),
        };
        return cfg;
    }

    OpponentConfig getOpponentConfig() {
        OpponentConfig cfg{
            .speed = YerbEngine::ConfigAdapter::floatOr(7.f, m_demoStore,
                                                        "opponentConfig.speed"),
            .deadZone = YerbEngine::ConfigAdapter::floatOr(
                14.f, m_demoStore, "opponentConfig.deadZone"),
            .reactionDistance = YerbEngine::ConfigAdapter::floatOr(
                0.62f, m_demoStore, "opponentConfig.reactionDistance"),
        };
        return cfg;
    }
};
