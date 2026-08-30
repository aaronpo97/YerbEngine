#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <Helpers/Vec2.hpp>
#include <string>

namespace PongTextHelpers {

YerbEngine::Vec2 measureText(TTF_Font* font, std::string const& text);

void renderCenteredText(SDL_Renderer* renderer, TTF_Font* font,
                        std::string const& text, SDL_Color const& color,
                        YerbEngine::Vec2 const& centerTop);

}  // namespace PongTextHelpers
