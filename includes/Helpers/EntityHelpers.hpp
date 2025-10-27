#pragma once

#include "../EntityManagement/Entity.hpp"
#include "../EntityManagement/EntityManager.hpp"
#include "../Helpers/MathHelpers.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace EntityHelpers {
    EntityList getEntitiesInRadius(std::shared_ptr<Entity> const &entity,
                                   EntityList const              &candidates,
                                   float const                   &radius);
} // namespace EntityHelpers
