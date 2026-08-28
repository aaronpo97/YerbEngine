#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <iostream>

#include "./Vec2.hpp"

namespace YerbEngine {

namespace TextHelpers {
void renderLineOfText(SDL_Renderer* renderer, TTF_Font* font,
                      std::string const& text, SDL_Color const& color,
                      Vec2 const& position);
}  // namespace TextHelpers

}  // namespace YerbEngine
