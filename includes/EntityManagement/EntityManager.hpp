#pragma once

#include "./Entity.hpp"
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

using EntityList = std::list<std::shared_ptr<Entity>>;
using EntityMap  = std::unordered_map<EntityTags, EntityList>;

class EntityManager {
  EntityList m_entities;
  EntityList m_toAdd;
  EntityMap  m_entityMap;
  size_t     m_totalEntities = 0;

public:
  EntityManager() = default;
  ~EntityManager() = default;

  // No copying or moving allowed
  EntityManager(const EntityManager &) = delete;
  EntityManager &operator=(const EntityManager &) = delete;
  EntityManager(EntityManager &&) = delete;
  EntityManager &operator=(EntityManager &&) = delete;

  std::shared_ptr<Entity> addEntity(const EntityTags tag);
  EntityList             &getEntities();
  EntityList             &getEntities(const EntityTags tag);
  void                    update();
};
