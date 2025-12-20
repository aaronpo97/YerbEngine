#include <EntityManagement/Entity.hpp>
#include <EntityManagement/EntityManager.hpp>
#include <ranges>

namespace YerbEngine {

    std::shared_ptr<Entity> EntityManager::addEntity(EntityTags const tag) {
        auto entityToAdd = std::shared_ptr<Entity>(
            new Entity(m_totalEntities++, tag, m_components));
        m_toAdd.push_back(entityToAdd);
        return entityToAdd;
    }

    EntityList &EntityManager::getEntities() { return m_entities; }

    EntityList &EntityManager::getEntities(EntityTags const tag) {
        return m_entityMap[tag];
    }

    ComponentRegistry &EntityManager::components() { return *m_components; }

    ComponentRegistry const &EntityManager::components() const {
        return *m_components;
    }

    void EntityManager::update() {
        auto removeDeadEntities = [this](EntityList &entityVec) -> void {
            std::erase_if(entityVec, [this](auto &entity) {
                if (!entity->isActive()) {
                    if (m_components) {
                        m_components->removeAllForEntity(entity->id());
                    }
                    return true;
                }
                return false;
            });
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

} // namespace YerbEngine
