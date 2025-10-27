#include <YerbEngine/Helpers/MovementHelpers.hpp>

constexpr float BASE_MOVEMENT_MULTIPLIER = 50.0f;

namespace MovementHelpers {

    void moveEnemies(std::shared_ptr<Entity> const &entity,
                     EnemyConfig const             &enemyConfig,
                     float const                   &deltaTime) {

        if (entity == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
            return;
        }

        EntityTags const entityTag = entity->tag();
        if (entityTag != EntityTags::Enemy) {
            return;
        }

        std::shared_ptr<CTransform> const &entityCTransform =
            entity->getComponent<CTransform>();
        if (entityCTransform == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Entity with ID %zu lacks a transform component.",
                         entity->id());
            return;
        }

        Vec2       &position = entityCTransform->topLeftCornerPos;
        Vec2 const &velocity = entityCTransform->velocity;

        position += velocity * (enemyConfig.speed *
                                (deltaTime * BASE_MOVEMENT_MULTIPLIER));
    }

    void moveSpeedBoosts(std::shared_ptr<Entity> const &entity,
                         SpeedEffectConfig const       &speedBoostEffectConfig,
                         float const                   &deltaTime) {
        if (entity == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
            return;
        }

        EntityTags const entityTag = entity->tag();
        if (entityTag != EntityTags::SpeedBoost) {
            return;
        }

        std::shared_ptr<CTransform> const &entityCTransform =
            entity->getComponent<CTransform>();
        if (entityCTransform == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Entity with ID %zu lacks a transform component.",
                         entity->id());
            return;
        }

        Vec2       &position = entityCTransform->topLeftCornerPos;
        Vec2 const &velocity = entityCTransform->velocity;

        position += velocity * deltaTime * speedBoostEffectConfig.speed *
                    BASE_MOVEMENT_MULTIPLIER;
    }

    void movePlayer(std::shared_ptr<Entity> const &entity,
                    PlayerConfig const            &playerConfig,
                    float const                   &deltaTime) {
        if (entity == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
            return;
        }

        EntityTags const entityTag = entity->tag();
        if (entityTag != EntityTags::Player) {
            return;
        }

        std::shared_ptr<CTransform> const &entityCTransform =
            entity->getComponent<CTransform>();
        if (entityCTransform == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Entity with ID %zu lacks a transform component.",
                         entity->id());
            return;
        }

        std::shared_ptr<CInput> const &entityCInput =
            entity->getComponent<CInput>();
        if (entityCInput == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Entity with ID %zu lacks an input component.",
                         entity->id());
            return;
        }

        Vec2 &position = entityCTransform->topLeftCornerPos;
        Vec2 &velocity = entityCTransform->velocity;

        velocity = {0, 0};

        if (entityCInput->forward) {
            velocity.y = -1;
        }
        if (entityCInput->backward) {
            velocity.y = 1;
        }
        if (entityCInput->left) {
            velocity.x = -1;
        }
        if (entityCInput->right) {
            velocity.x = 1;
        }

        velocity.normalize();

        std::shared_ptr<CEffects> const &entityEffects =
            entity->getComponent<CEffects>();

        float effectMultiplier = 1;
        if (entityEffects->hasEffect(EffectTypes::Speed)) {
            effectMultiplier = playerConfig.speedBoostMultiplier;
        }

        if (entityEffects->hasEffect(EffectTypes::Slowness)) {
            effectMultiplier = playerConfig.slownessMultiplier;
        }

        velocity *= effectMultiplier * deltaTime * BASE_MOVEMENT_MULTIPLIER *
                    playerConfig.baseSpeed;
        position += velocity;
    }

    void moveSlownessDebuffs(std::shared_ptr<Entity> const &entity,
                             SlownessEffectConfig const &slownessEffectConfig,
                             float const                &deltaTime) {

        if (entity == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
            return;
        }

        EntityTags const entityTag = entity->tag();
        if (entityTag != EntityTags::SlownessDebuff) {
            return;
        }

        std::shared_ptr<CTransform> const &entityCTransform =
            entity->getComponent<CTransform>();
        std::shared_ptr<CShape> const &entityCShape =
            entity->getComponent<CShape>();

        if (entityCTransform == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Entity with ID %zu lacks a transform component.",
                         entity->id());

            return;
        }

        if (entityCShape == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Entity with ID %zu lacks a shape component.",
                         entity->id());

            return;
        }

        Vec2       &position = entityCTransform->topLeftCornerPos;
        Vec2 const &velocity = entityCTransform->velocity;

        position += velocity * deltaTime * slownessEffectConfig.speed *
                    BASE_MOVEMENT_MULTIPLIER;
    }

    void moveBullets(std::shared_ptr<Entity> const &entity,
                     float const                   &deltaTime) {
        if (entity == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
            return;
        }

        EntityTags const entityTag = entity->tag();
        if (entityTag != EntityTags::Bullet) {
            return;
        }

        std::shared_ptr<CTransform> const &entityCTransform =
            entity->getComponent<CTransform>();
        if (entityCTransform == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Entity with ID %zu lacks a transform component.",
                         entity->id());
            return;
        }

        Vec2       &position = entityCTransform->topLeftCornerPos;
        Vec2 const &velocity = entityCTransform->velocity;

        constexpr float BULLET_MOVEMENT_MULTIPLIER = 3.0f;
        position += velocity * (deltaTime * BULLET_MOVEMENT_MULTIPLIER *
                                BASE_MOVEMENT_MULTIPLIER);
    }
    void moveItems(std::shared_ptr<Entity> const &entity,
                   float const                   &deltaTime) {
        if (entity == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
            return;
        }

        EntityTags const entityTag = entity->tag();

        if (entityTag != EntityTags::Item) {
            return;
        }

        std::shared_ptr<CTransform> const &entityCTransform =
            entity->getComponent<CTransform>();

        if (entityCTransform == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Entity with ID %zu lacks a transform component.",
                         entity->id());
            return;
        }

        Vec2 &position = entityCTransform->topLeftCornerPos;

        // Use deltaTime to maintain consistent movement speed
        constexpr float ITEM_MOVEMENT_MULTIPLIER = .9f;
        float const     time = static_cast<float>(SDL_GetTicks64()) / 1000.0f;
        // Entity id will be odd when the last bit is 1
        bool const ENTITY_ID_ODD = entity->id() & 1;

        if (ENTITY_ID_ODD) {
            position.x +=
                std::cos(time) * (ITEM_MOVEMENT_MULTIPLIER * deltaTime *
                                  BASE_MOVEMENT_MULTIPLIER);
            position.y +=
                std::sin(time) * (ITEM_MOVEMENT_MULTIPLIER * deltaTime *
                                  BASE_MOVEMENT_MULTIPLIER);

        } else {
            position.x +=
                std::sin(time) * (ITEM_MOVEMENT_MULTIPLIER * deltaTime *
                                  BASE_MOVEMENT_MULTIPLIER);
            position.y +=
                std::cos(time) * (ITEM_MOVEMENT_MULTIPLIER * deltaTime *
                                  BASE_MOVEMENT_MULTIPLIER);
        }
    }
} // namespace MovementHelpers
