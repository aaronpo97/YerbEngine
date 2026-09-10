#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>

#include "./Vec2.hpp"

namespace YerbEngine {

namespace TextHelpers {
/** Which edge of the text the given position refers to. */
enum class TextAlign { Left, Center, Right };

/** The size the text would occupy when rendered in the given font. */
Vec2 measureText(TTF_Font* font, std::string const& text);

/**
 * Draws a single line of text with its top edge at position.y().
 *
 * position.x() is the left edge, the horizontal centre, or the right edge
 * of the line, following align.
 */
void renderLineOfText(SDL_Renderer* renderer, TTF_Font* font,
                      std::string const& text, SDL_Color const& color,
                      Vec2 const& position, TextAlign align = TextAlign::Left);
}  // namespace TextHelpers

}  // namespace YerbEngine
