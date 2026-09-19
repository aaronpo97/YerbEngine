#pragma once

#include <cstdint>

namespace YerbEngine {

/**
 * An entity's kind, as a small integer each game gives meaning to.
 *
 * The engine names only the few tags that are common to every game; a game
 * defines its own kinds from EntityTags::FirstGameTag upwards, for example:
 *
 *     namespace MyGameTags {
 *     inline constexpr YerbEngine::EntityTag Enemy =
 *         YerbEngine::EntityTags::FirstGameTag + 0;
 *     inline constexpr YerbEngine::EntityTag Pickup =
 *         YerbEngine::EntityTags::FirstGameTag + 1;
 *     }
 */
using EntityTag = std::uint32_t;

namespace EntityTags {
inline constexpr EntityTag Default = 0;
inline constexpr EntityTag Player = 1;
inline constexpr EntityTag Wall = 2;

/** The first value a game may claim for a kind of its own. */
inline constexpr EntityTag FirstGameTag = 16;
}  // namespace EntityTags

}  // namespace YerbEngine
