#pragma once

#include "./Components.hpp"
#include <memory>
#include <ostream>
namespace YerbEngine {
    enum class EntityTags {
        Player,
        Wall,
        SpeedBoost,
        SlownessDebuff,
        Enemy,
        Bullet,
        Item,
        Default
    };

    inline std::ostream &operator<<(std::ostream     &os,
                                    EntityTags const &tag) {
        switch (tag) {
        case EntityTags::Player:
            os << "Player";
            break;
        case EntityTags::Wall:
            os << "Wall";
            break;
        case EntityTags::SpeedBoost:
            os << "SpeedBoost";
            break;
        case EntityTags::SlownessDebuff:
            os << "SlownessDebuff";
            break;
        case EntityTags::Enemy:
            os << "Enemy";
            break;
        case EntityTags::Bullet:
            os << "Bullet";
            break;
        case EntityTags::Item:
            os << "Item";
            break;
        case EntityTags::Default:
            os << "Default";
            break;
        }
        return os;
    }

    using EntityComponents = std::tuple<std::shared_ptr<CTransform>,
                                        std::shared_ptr<CShape>,
                                        std::shared_ptr<CInput>,
                                        std::shared_ptr<CLifespan>,
                                        std::shared_ptr<CEffects>,
                                        std::shared_ptr<CBounceTracker>,
                                        std::shared_ptr<CSprite>>;

    class Entity {
        friend class EntityManager;
        bool       m_active = true;
        size_t     m_id     = 0;
        EntityTags m_tag    = EntityTags::Default;

        EntityComponents m_components;

        Entity(size_t     id,
               EntityTags tag);

      public:
        // private member access functions
        bool       isActive() const;
        EntityTags tag() const;
        size_t     id() const;
        void       destroy();
        Vec2       getCenterPos() const;

        template <typename ComponentType>
        std::shared_ptr<ComponentType> getComponent() const;
        template <typename ComponentType>
        void setComponent(std::shared_ptr<ComponentType> component);
        template <typename ComponentType>
        void removeComponent();
        template <typename ComponentType>
        bool hasComponent() const;
    };

    template <typename ComponentType>
    std::shared_ptr<ComponentType> Entity::getComponent() const {
        auto component = std::get<std::shared_ptr<ComponentType>>(m_components);
        return component;
    }

    template <typename ComponentType>
    void Entity::setComponent(std::shared_ptr<ComponentType> component) {
        std::get<std::shared_ptr<ComponentType>>(m_components) = component;
    }

    template <typename ComponentType>
    void Entity::removeComponent() {
        std::get<std::shared_ptr<ComponentType>>(m_components) = nullptr;
    }

    template <typename ComponentType>
    bool Entity::hasComponent() const {
        auto hasComponent =
            std::get<std::shared_ptr<ComponentType>>(m_components) != nullptr;
        return hasComponent;
    }
} // namespace YerbEngine