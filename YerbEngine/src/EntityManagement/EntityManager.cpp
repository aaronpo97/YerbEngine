#include <EntityManagement/Entity.hpp>
#include <EntityManagement/EntityManager.hpp>
#include <ranges>

std::shared_ptr<Entity> EntityManager::addEntity(EntityTags const tag) {
    auto entityToAdd =
        std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_toAdd.push_back(entityToAdd);
    return entityToAdd;
}

EntityList &EntityManager::getEntities() { return m_entities; }

EntityList &EntityManager::getEntities(EntityTags const tag) {
    return m_entityMap[tag];
}

void EntityManager::update() {
    auto removeDeadEntities = [](EntityList &entityVec) -> void {
        std::erase_if(entityVec,
                      [](auto &entity) { return !entity->isActive(); });
    };

    for (std::shared_ptr<Entity> const &entity : m_toAdd) {
        m_entities.push_back(entity);
        m_entityMap[entity->tag()].push_back(entity);
    }

    removeDeadEntities(m_entities);

    for (auto &entityVec : m_entityMap | std::views::values) {
        removeDeadEntities(entityVec);
    }

    m_toAdd.clear();
}