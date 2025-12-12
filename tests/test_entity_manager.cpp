#include <boost/test/unit_test.hpp>

#include "Timer.hpp"
#include <EntityManagement/Entity.hpp>
#include <EntityManagement/EntityManager.hpp>

using namespace YerbEngine;

BOOST_AUTO_TEST_SUITE(EntityManagerTests)

// Test EntityManager::addEntity
BOOST_AUTO_TEST_CASE(test_add_entity) {
    Timer         timer("Add entity");
    EntityManager manager;

    constexpr size_t numEntities = 1000;

    for (size_t i = 0; i < numEntities; ++i) {
        auto entity = manager.addEntity(EntityTags::Enemy);
        BOOST_CHECK(entity != nullptr);
        BOOST_CHECK_EQUAL(entity->tag(), EntityTags::Enemy);
        BOOST_CHECK_EQUAL(entity->id(), i);
    }

    manager.update();

    BOOST_CHECK_EQUAL(manager.getEntities().size(), numEntities);
}

// Test EntityManager::update adds entities to main list
BOOST_AUTO_TEST_CASE(test_update_adds_entities) {
    Timer         timer("Update adds entities");
    EntityManager manager;

    manager.addEntity(EntityTags::Player);
    manager.addEntity(EntityTags::Enemy);

    // Before update, main entity list should be empty
    BOOST_CHECK_EQUAL(manager.getEntities().size(), 0);

    manager.update();

    // After update, entities should be in main list
    BOOST_CHECK_EQUAL(manager.getEntities().size(), 2);
}

// Test EntityManager::getEntities by tag
BOOST_AUTO_TEST_CASE(test_get_entities_by_tag) {
    Timer         timer("Get entities by tag");
    EntityManager manager;

    manager.addEntity(EntityTags::Player);
    manager.addEntity(EntityTags::Enemy);
    manager.addEntity(EntityTags::Enemy);
    manager.addEntity(EntityTags::Bullet);

    manager.update();

    auto &players = manager.getEntities(EntityTags::Player);
    auto &enemies = manager.getEntities(EntityTags::Enemy);
    auto &bullets = manager.getEntities(EntityTags::Bullet);
    auto &walls   = manager.getEntities(EntityTags::Wall);

    BOOST_CHECK_EQUAL(players.size(), 1);
    BOOST_CHECK_EQUAL(enemies.size(), 2);
    BOOST_CHECK_EQUAL(bullets.size(), 1);
    BOOST_CHECK_EQUAL(walls.size(), 0);
}

// Test EntityManager removes dead entities
BOOST_AUTO_TEST_CASE(test_remove_dead_entities) {
    Timer         timer("Remove dead entities");
    EntityManager manager;

    for (size_t i = 0; i < 1000; ++i) {
        auto entity = manager.addEntity(EntityTags::Enemy);
    }

    manager.update();

    // Destroy half the entities
    size_t count = 0;
    for (auto &entity : manager.getEntities()) {
        if (count % 2 == 0) {
            entity->destroy();
        }
        ++count;
    }

    manager.update();

    BOOST_CHECK_EQUAL(manager.getEntities().size(), 500);
    // Verify the remaining entities are active
    for (auto &entity : manager.getEntities()) {
        BOOST_CHECK(entity->isActive());
    }
}

// Test EntityManager increments total entities correctly
BOOST_AUTO_TEST_CASE(test_entity_id_increment) {
    Timer         timer("Entity ID increment");
    EntityManager manager;

    auto entity1 = manager.addEntity(EntityTags::Player);
    auto entity2 = manager.addEntity(EntityTags::Enemy);
    auto entity3 = manager.addEntity(EntityTags::Bullet);

    BOOST_CHECK_EQUAL(entity1->id(), 0);
    BOOST_CHECK_EQUAL(entity2->id(), 1);
    BOOST_CHECK_EQUAL(entity3->id(), 2);

    manager.update();

    // Add more entities after update
    auto entity4 = manager.addEntity(EntityTags::Item);
    BOOST_CHECK_EQUAL(entity4->id(), 3);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(EntityTests)

// Test Entity::isActive default state
BOOST_AUTO_TEST_CASE(test_entity_is_active_by_default) {
    Timer         timer("Entity is active by default");
    EntityManager manager;
    auto          entity = manager.addEntity(EntityTags::Player);

    BOOST_CHECK(entity->isActive());
}

// Test Entity::destroy
BOOST_AUTO_TEST_CASE(test_entity_destroy) {
    Timer         timer("Entity destroy");
    EntityManager manager;
    auto          entity = manager.addEntity(EntityTags::Player);

    BOOST_CHECK(entity->isActive());

    entity->destroy();

    BOOST_CHECK(!entity->isActive());
}

// Test Entity::tag
BOOST_AUTO_TEST_CASE(test_entity_tag) {
    Timer         timer("Entity tag");
    EntityManager manager;

    auto player = manager.addEntity(EntityTags::Player);
    auto enemy  = manager.addEntity(EntityTags::Enemy);
    auto bullet = manager.addEntity(EntityTags::Bullet);

    BOOST_CHECK_EQUAL(player->tag(), EntityTags::Player);
    BOOST_CHECK_EQUAL(enemy->tag(), EntityTags::Enemy);
    BOOST_CHECK_EQUAL(bullet->tag(), EntityTags::Bullet);
}

// Test Entity::id
BOOST_AUTO_TEST_CASE(test_entity_id) {
    Timer         timer("Entity id");
    EntityManager manager;

    auto entity1 = manager.addEntity(EntityTags::Player);
    auto entity2 = manager.addEntity(EntityTags::Enemy);

    BOOST_CHECK_EQUAL(entity1->id(), 0);
    BOOST_CHECK_EQUAL(entity2->id(), 1);
    BOOST_CHECK_NE(entity1->id(), entity2->id());
}

// Test Entity component system - setComponent and getComponent
BOOST_AUTO_TEST_CASE(test_entity_set_and_get_component) {
    Timer         timer("Entity set and get component");
    EntityManager manager;
    auto          entity = manager.addEntity(EntityTags::Player);

    // Initially, component should be null
    BOOST_CHECK(!entity->hasComponent<CTransform>());
    BOOST_CHECK(entity->getComponent<CTransform>() == nullptr);

    // Add a transform component
    auto transform =
        std::make_shared<CTransform>(Vec2{100.0f, 200.0f}, Vec2{5.0f, 10.0f});
    entity->setComponent(transform);

    // Now component should exist
    BOOST_CHECK(entity->hasComponent<CTransform>());
    BOOST_CHECK(entity->getComponent<CTransform>() != nullptr);

    // Verify component data
    auto retrievedTransform = entity->getComponent<CTransform>();
    BOOST_CHECK_EQUAL(retrievedTransform->topLeftCornerPos.x(), 100.0f);
    BOOST_CHECK_EQUAL(retrievedTransform->topLeftCornerPos.y(), 200.0f);
    BOOST_CHECK_EQUAL(retrievedTransform->velocity.x(), 5.0f);
    BOOST_CHECK_EQUAL(retrievedTransform->velocity.y(), 10.0f);
}

// Test Entity::hasComponent
BOOST_AUTO_TEST_CASE(test_entity_has_component) {
    Timer         timer("Entity has component");
    EntityManager manager;
    auto          entity = manager.addEntity(EntityTags::Player);

    BOOST_CHECK(!entity->hasComponent<CTransform>());
    BOOST_CHECK(!entity->hasComponent<CInput>());
    BOOST_CHECK(!entity->hasComponent<CLifespan>());

    entity->setComponent(std::make_shared<CTransform>());
    BOOST_CHECK(entity->hasComponent<CTransform>());
    BOOST_CHECK(!entity->hasComponent<CInput>());

    entity->setComponent(std::make_shared<CInput>());
    BOOST_CHECK(entity->hasComponent<CTransform>());
    BOOST_CHECK(entity->hasComponent<CInput>());
}

// Test Entity::removeComponent
BOOST_AUTO_TEST_CASE(test_entity_remove_component) {
    Timer         timer("Entity remove component");
    EntityManager manager;
    auto          entity = manager.addEntity(EntityTags::Player);

    auto transform =
        std::make_shared<CTransform>(Vec2{100.0f, 200.0f}, Vec2{5.0f, 10.0f});
    entity->setComponent(transform);

    BOOST_CHECK(entity->hasComponent<CTransform>());

    entity->removeComponent<CTransform>();

    BOOST_CHECK(!entity->hasComponent<CTransform>());
    BOOST_CHECK(entity->getComponent<CTransform>() == nullptr);
}

// Test multiple components on same entity
BOOST_AUTO_TEST_CASE(test_entity_multiple_components) {
    Timer         timer("Entity multiple components");
    EntityManager manager;
    auto          entity = manager.addEntity(EntityTags::Player);

    auto transform =
        std::make_shared<CTransform>(Vec2{100.0f, 200.0f}, Vec2{5.0f, 10.0f});
    auto input    = std::make_shared<CInput>();
    auto lifespan = std::make_shared<CLifespan>(60);

    entity->setComponent(transform);
    entity->setComponent(input);
    entity->setComponent(lifespan);

    BOOST_CHECK(entity->hasComponent<CTransform>());
    BOOST_CHECK(entity->hasComponent<CInput>());
    BOOST_CHECK(entity->hasComponent<CLifespan>());

    // Verify each component has correct data
    auto retrievedTransform = entity->getComponent<CTransform>();
    BOOST_CHECK_EQUAL(retrievedTransform->topLeftCornerPos.x(), 100.0f);

    auto retrievedLifespan = entity->getComponent<CLifespan>();
    BOOST_CHECK_EQUAL(retrievedLifespan->lifespan, 60);
}

// Test entity component modification
BOOST_AUTO_TEST_CASE(test_entity_component_modification) {
    Timer         timer("Entity component modification");
    EntityManager manager;
    auto          entity = manager.addEntity(EntityTags::Player);

    auto transform =
        std::make_shared<CTransform>(Vec2{100.0f, 200.0f}, Vec2{5.0f, 10.0f});
    entity->setComponent(transform);

    // Modify the component through the entity
    auto retrievedTransform = entity->getComponent<CTransform>();
    retrievedTransform->topLeftCornerPos.setX(150.0f);
    retrievedTransform->velocity.setY(20.0f);

    // Verify modifications persist
    auto modifiedTransform = entity->getComponent<CTransform>();
    BOOST_CHECK_EQUAL(modifiedTransform->topLeftCornerPos.x(), 150.0f);
    BOOST_CHECK_EQUAL(modifiedTransform->velocity.y(), 20.0f);
}

BOOST_AUTO_TEST_SUITE_END()

// Integration tests
BOOST_AUTO_TEST_SUITE(EntityManagerIntegrationTests)

// Test complex scenario: create, destroy, update
BOOST_AUTO_TEST_CASE(test_complex_entity_lifecycle) {
    Timer         timer("Complex entity lifecycle");
    EntityManager manager;

    // Create initial entities
    auto player = manager.addEntity(EntityTags::Player);
    auto enemy1 = manager.addEntity(EntityTags::Enemy);
    auto enemy2 = manager.addEntity(EntityTags::Enemy);
    auto bullet = manager.addEntity(EntityTags::Bullet);

    manager.update();

    BOOST_CHECK_EQUAL(manager.getEntities().size(), 4);
    BOOST_CHECK_EQUAL(manager.getEntities(EntityTags::Enemy).size(), 2);

    // Destroy one enemy
    enemy1->destroy();
    manager.update();

    BOOST_CHECK_EQUAL(manager.getEntities().size(), 3);
    BOOST_CHECK_EQUAL(manager.getEntities(EntityTags::Enemy).size(), 1);

    // Add new entities
    auto bullet2 = manager.addEntity(EntityTags::Bullet);
    auto bullet3 = manager.addEntity(EntityTags::Bullet);

    manager.update();

    BOOST_CHECK_EQUAL(manager.getEntities().size(), 5);
    BOOST_CHECK_EQUAL(manager.getEntities(EntityTags::Bullet).size(), 3);

    // Destroy all bullets
    for (auto &bulletEntity : manager.getEntities(EntityTags::Bullet)) {
        bulletEntity->destroy();
    }

    manager.update();

    BOOST_CHECK_EQUAL(manager.getEntities().size(), 2);
    BOOST_CHECK_EQUAL(manager.getEntities(EntityTags::Bullet).size(), 0);
}

// Test entity with components throughout lifecycle
BOOST_AUTO_TEST_CASE(test_entity_components_lifecycle) {
    Timer         timer("Entity components lifecycle");
    EntityManager manager;

    auto enemy = manager.addEntity(EntityTags::Enemy);
    enemy->setComponent(
        std::make_shared<CTransform>(Vec2{50.0f, 50.0f}, Vec2{1.0f, 1.0f}));
    enemy->setComponent(std::make_shared<CLifespan>(120));

    manager.update();

    BOOST_CHECK(enemy->hasComponent<CTransform>());
    BOOST_CHECK(enemy->hasComponent<CLifespan>());

    // Simulate lifespan countdown
    auto lifespan = enemy->getComponent<CLifespan>();
    lifespan->lifespan -= 60;
    BOOST_CHECK_EQUAL(lifespan->lifespan, 60);

    manager.update();

    // Entity should still exist
    BOOST_CHECK_EQUAL(manager.getEntities().size(), 1);
    BOOST_CHECK(enemy->isActive());

    // Destroy entity
    enemy->destroy();
    manager.update();

    BOOST_CHECK_EQUAL(manager.getEntities().size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
