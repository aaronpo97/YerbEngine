#include <EntityManagement/Entity.hpp>

namespace YerbEngine {

    Entity::Entity(size_t const                     id,
                   EntityTags const                 tag,
                   std::shared_ptr<ComponentRegistry> registry)
        : m_id(id),
          m_tag(tag),
          m_registry(std::move(registry)) {}

    bool Entity::isActive() const { return m_active; }

    EntityTags Entity::tag() const { return m_tag; }

    size_t Entity::id() const { return m_id; }

    void Entity::destroy() { m_active = false; }

    Vec2 Entity::getCenterPos() const {
        std::shared_ptr<Components::CTransform> const &cTransform =
            getComponent<Components::CTransform>();
        std::shared_ptr<Components::CShape> const &cShape = getComponent<Components::CShape>();

        if (cTransform == nullptr || cShape == nullptr) {
            SDL_LogError(
                SDL_LOG_CATEGORY_ERROR,
                "Entity lacks a transform or shape component. Unable to "
                "calculate center "
                "position.");
            return Vec2{0, 0};
        }

        Vec2 const &pos = cTransform->topLeftCornerPos;
        Vec2 const &center =
            pos + Vec2(static_cast<float>(cShape->rect.w) / 2.0f,
                       static_cast<float>(cShape->rect.h) / 2.0f);
        return center;
    }

} // namespace YerbEngine
