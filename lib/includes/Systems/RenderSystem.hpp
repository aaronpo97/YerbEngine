#pragma once

#include <SDL3/SDL.h>

#include <AssetManagement/TextureManager.hpp>
#include <EntityManagement/EntityManager.hpp>
#include <SystemManagement/VideoManager.hpp>

namespace YerbEngine {

namespace RenderSystem {
constexpr SDL_Color BACKGROUND_COLOR = {.r = 0, .g = 0, .b = 0, .a = 255};

/** Fills the frame with a single colour, ready to be drawn over. */
void clear(VideoManager& videoManager,
           SDL_Color const& color = BACKGROUND_COLOR);

/**
 * Draws every entity that carries both a shape and a transform: as a
 * texture when the entity has a sprite, as a filled rectangle otherwise.
 *
 * The shape's rect follows the transform, so entities render wherever the
 * movement systems last put them.
 */
void renderEntities(EntityManager& entityManager, VideoManager& videoManager,
                    TextureManager& textureManager);

/** Shows the frame that has been drawn so far. */
void present(VideoManager& videoManager);
}  // namespace RenderSystem

}  // namespace YerbEngine
