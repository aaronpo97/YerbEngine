#include <Helpers/PongTextHelpers.hpp>
#include <Helpers/TextHelpers.hpp>

using YerbEngine::Vec2;

namespace PongTextHelpers {

Vec2 measureText(TTF_Font* font, std::string const& text) {
   if (font == nullptr || text.empty()) {
      return Vec2{0, 0};
   }

   int width = 0;
   int height = 0;
   if (TTF_SizeText(font, text.c_str(), &width, &height) != 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s",
                   TTF_GetError());
      return Vec2{0, 0};
   }

   return Vec2{static_cast<float>(width), static_cast<float>(height)};
}

void renderCenteredText(SDL_Renderer* renderer, TTF_Font* font,
                        std::string const& text, SDL_Color const& color,
                        Vec2 const& centerTop) {
   if (font == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Cannot render '%s' as the font is null.", text.c_str());
      return;
   }

   if (text.empty()) {
      return;
   }

   Vec2 const size = measureText(font, text);
   Vec2 const topLeft{centerTop.x() - size.x() / 2.0f, centerTop.y()};

   YerbEngine::TextHelpers::renderLineOfText(renderer, font, text, color,
                                             topLeft);
}

}  // namespace PongTextHelpers
