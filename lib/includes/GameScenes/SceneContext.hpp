#pragma once

#include <random>

class DemoConfigAdapter;

namespace YerbEngine {

    class ActionQueue;
    class EntityManager;
    class GameEngine;

    struct SceneContext {
        EntityManager    &entities;
        GameEngine       &engine;
        DemoConfigAdapter *demoConfig; // optional per-scene
        std::mt19937     &rng;
        ActionQueue      &actions;
        float             deltaTime;
    };

} // namespace YerbEngine
