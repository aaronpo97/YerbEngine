#pragma once

#include <YerbEngine/EntityManagement/Entity.hpp>
#include <YerbEngine/EntityManagement/EntityManager.hpp>
#include <YerbEngine/Helpers/MathHelpers.hpp>
#include <algorithm>
#include <memory>
#include <vector>

namespace EntityHelpers {
    EntityList getEntitiesInRadius(std::shared_ptr<Entity> const &entity,
                                   EntityList const              &candidates,
                                   float const                   &radius);
} // namespace EntityHelpers
