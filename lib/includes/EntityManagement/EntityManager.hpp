#pragma once

#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include "./ComponentRegistry.hpp"
#include "./Entity.hpp"

namespace YerbEngine {

using EntityList = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::unordered_map<EntityTag, EntityList>;

class EntityManager {
   EntityList m_entities;
   EntityList m_toAdd;
   EntityMap m_entityMap;
   size_t m_totalEntities = 0;
   std::shared_ptr<ComponentRegistry> m_components =
       std::make_shared<ComponentRegistry>();

  public:
   EntityManager() = default;
   ~EntityManager() = default;

   // No copying or moving allowed
   EntityManager(EntityManager const&) = delete;
   EntityManager& operator=(EntityManager const&) = delete;
   EntityManager(EntityManager&&) = delete;
   EntityManager& operator=(EntityManager&&) = delete;

   std::shared_ptr<Entity> addEntity(EntityTag tag);
   EntityList& getEntities();
   EntityList& getEntities(EntityTag tag);

   ComponentRegistry& components();
   ComponentRegistry const& components() const;
   void update();
};

}  // namespace YerbEngine
