#pragma once

#include <memory>

#include "./ComponentRegistry.hpp"
#include "./Components.hpp"
#include "./EntityTag.hpp"
namespace YerbEngine {
class Entity {
   friend class EntityManager;
   bool m_active = true;
   size_t m_id = 0;
   EntityTag m_tag = EntityTags::Default;
   std::weak_ptr<ComponentRegistry> m_registry;

   Entity(size_t id, EntityTag tag,
          std::shared_ptr<ComponentRegistry> registry);

  public:
   // private member access functions
   bool isActive() const;
   EntityTag tag() const;
   size_t id() const;
   void destroy();
   Vec2 getCenterPos() const;

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
   auto registry = m_registry.lock();
   if (!registry) {
      return nullptr;
   }
   return registry->get<ComponentType>(m_id);
}

template <typename ComponentType>
void Entity::setComponent(std::shared_ptr<ComponentType> component) {
   auto registry = m_registry.lock();
   if (!registry) {
      return;
   }
   registry->emplace<ComponentType>(m_id, std::move(component));
}

template <typename ComponentType>
void Entity::removeComponent() {
   auto registry = m_registry.lock();
   if (!registry) {
      return;
   }
   registry->remove<ComponentType>(m_id);
}

template <typename ComponentType>
bool Entity::hasComponent() const {
   auto registry = m_registry.lock();
   if (!registry) {
      return false;
   }
   return registry->contains<ComponentType>(m_id);
}
}  // namespace YerbEngine
