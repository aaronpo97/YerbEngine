#include <EntityManagement/Entity.hpp>
#include <Helpers/CollisionHelpers.hpp>

#include <bitset>

namespace YerbEngine {

    enum Boundaries : Uint8 { TOP, BOTTOM, LEFT, RIGHT };

    enum RelativePosition : Uint8 { ABOVE, BELOW, LEFT_OF, RIGHT_OF };

    namespace CollisionHelpers {
        std::bitset<4> detectOutOfBounds(std::shared_ptr<Entity> const &entity,
                                         Vec2 const &window_size) {

            std::shared_ptr<Components::CTransform> const &cTransform =
                entity->getComponent<Components::CTransform>();
            std::shared_ptr<Components::CShape> const &cShape =
                entity->getComponent<Components::CShape>();

            if (!cTransform || !cShape) {
                SDL_LogError(
                    SDL_LOG_CATEGORY_SYSTEM,
                    "Entity with ID %zu and tag %u lacks a transform or "
                    "shape component.",
                    entity->id(), entity->tag());

                return {};
            }

            Vec2 const &topLeftCorner = cTransform->topLeftCornerPos;
            auto const  rectHeight    = static_cast<float>(cShape->rect.h);
            auto const  rectWidth     = static_cast<float>(cShape->rect.w);

            bool const collidesWithTop = topLeftCorner.y() <= 0;
            bool const collidesWithBottom =
                topLeftCorner.y() + rectHeight >= window_size.y();
            bool const collidesWithLeft = topLeftCorner.x() <= 0;
            bool const collidesWithRight =
                topLeftCorner.x() + rectWidth >= window_size.x();

            std::bitset<4> collidesWithBoundary;
            collidesWithBoundary[TOP]    = collidesWithTop;
            collidesWithBoundary[BOTTOM] = collidesWithBottom;
            collidesWithBoundary[LEFT]   = collidesWithLeft;
            collidesWithBoundary[RIGHT]  = collidesWithRight;

            return collidesWithBoundary;
        }

        Vec2 calculateOverlap(std::shared_ptr<Entity> const &entityA,
                              std::shared_ptr<Entity> const &entityB) {

            auto const &cShapeA = entityA->getComponent<Components::CShape>();
            auto const &cShapeB = entityB->getComponent<Components::CShape>();

            if (!cShapeA) {
                SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                             "Entity with ID %zu and tag %u lacks a collision "
                             "component.",
                             entityA->id(), entityA->tag());
                return Vec2{0, 0};
            }

            if (!cShapeB) {
                SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                             "Entity with ID %zu and tag %u lacks a collision "
                             "component.",
                             entityB->id(), entityB->tag());
                return Vec2{0, 0};
            }

            float const halfWidthA = static_cast<float>(cShapeA->rect.w) / 2.0f;
            float const halfHeightA =
                static_cast<float>(cShapeA->rect.h) / 2.0f;

            float const halfWidthB = static_cast<float>(cShapeB->rect.w) / 2.0f;
            float const halfHeightB =
                static_cast<float>(cShapeB->rect.h) / 2.0f;

            auto const halfSizeA = Vec2(halfWidthA, halfHeightA);
            auto const halfSizeB = Vec2(halfWidthB, halfHeightB);

            Vec2 const &centerA = entityA->getCenterPos();
            Vec2 const &centerB = entityB->getCenterPos();

            Vec2 const delta(std::abs(centerA.x() - centerB.x()),
                             std::abs(centerA.y() - centerB.y()));

            Vec2 const overlap{
                halfSizeA.x() + halfSizeB.x() - delta.x(),
                halfSizeA.y() + halfSizeB.y() - delta.y(),
            };

            return overlap;
        }

        bool calculateCollisionBetweenEntities(
            std::shared_ptr<Entity> const &entityA,
            std::shared_ptr<Entity> const &entityB) {
            Vec2 const overlap           = calculateOverlap(entityA, entityB);
            bool const collisionDetected = overlap.x() > 0 && overlap.y() > 0;
            return collisionDetected;
        }

        std::bitset<4>
        getPositionRelativeToEntity(std::shared_ptr<Entity> const &entityA,
                                    std::shared_ptr<Entity> const &entityB) {
            Vec2 const &centerA = entityA->getCenterPos();
            Vec2 const &centerB = entityB->getCenterPos();

            std::bitset<4> relativePosition;
            relativePosition[ABOVE]    = centerA.y() < centerB.y();
            relativePosition[BELOW]    = centerA.y() > centerB.y();
            relativePosition[LEFT_OF]  = centerA.x() < centerB.x();
            relativePosition[RIGHT_OF] = centerA.x() > centerB.x();

            return relativePosition;
        }

    } // namespace CollisionHelpers

} // namespace YerbEngine
