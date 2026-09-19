#pragma once

#include <EntityManagement/EntityTag.hpp>

/** The kinds of entity the shooting demo spawns. */
namespace DemoTags {
using YerbEngine::EntityTag;
namespace Engine = YerbEngine::EntityTags;

inline constexpr EntityTag Player = Engine::Player;
inline constexpr EntityTag Wall = Engine::Wall;
inline constexpr EntityTag Enemy = Engine::FirstGameTag + 0;
inline constexpr EntityTag Bullet = Engine::FirstGameTag + 1;
inline constexpr EntityTag Item = Engine::FirstGameTag + 2;
inline constexpr EntityTag SpeedBoost = Engine::FirstGameTag + 3;
inline constexpr EntityTag SlownessDebuff = Engine::FirstGameTag + 4;
}  // namespace DemoTags
