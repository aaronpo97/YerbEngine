#pragma once

#include "../Configuration/Config.hpp"
#include "../EntityManagement/Components.hpp"
#include "../EntityManagement/Entity.hpp"
#include "../Helpers/Vec2.hpp"
#include <SDL.h>
#include <memory>

namespace MovementHelpers {
    void moveEnemies(std::shared_ptr<Entity> const &entity,
                     EnemyConfig const             &enemyConfig,
                     float const                   &deltaTime);
    void moveSpeedBoosts(std::shared_ptr<Entity> const &entity,
                         SpeedEffectConfig const       &speedBoostEffectConfig,
                         float const                   &deltaTime);
    void movePlayer(std::shared_ptr<Entity> const &entity,
                    PlayerConfig const            &playerConfig,
                    float const                   &deltaTime);

    void moveSlownessDebuffs(std::shared_ptr<Entity> const &entity,
                             SlownessEffectConfig const &slownessEffectConfig,
                             float const                &deltaTime);

    void moveBullets(std::shared_ptr<Entity> const &entity,
                     float const                   &deltaTime);

    void moveItems(std::shared_ptr<Entity> const &entity,
                   float const                   &deltaTime);
} // namespace MovementHelpers
