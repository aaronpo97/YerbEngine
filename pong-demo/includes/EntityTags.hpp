#pragma once

#include <EntityManagement/EntityTag.hpp>

/** The kinds of entity a pong match holds. */
namespace PongTags {
using YerbEngine::EntityTag;

inline constexpr EntityTag Paddle = YerbEngine::EntityTags::Player;
inline constexpr EntityTag Ball = YerbEngine::EntityTags::FirstGameTag;
}  // namespace PongTags
