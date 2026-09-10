#include <Helpers/TextHelpers.hpp>

namespace YerbEngine {

namespace TextHelpers {

Vec2 measureText(TTF_Font* font, std::string const& text) {
   if (font == nullptr || text.empty()) {
      return Vec2{0, 0};
   }

   int width = 0;
   int height = 0;
   if (!TTF_GetStringSize(font, text.c_str(), text.size(), &width, &height)) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s",
                   SDL_GetError());
      return Vec2{0, 0};
   }

   return Vec2{static_cast<float>(width), static_cast<float>(height)};
}

void renderLineOfText(SDL_Renderer* renderer, TTF_Font* font,
                      std::string const& text, SDL_Color const& color,
                      Vec2 const& position, TextAlign const align) {
   if (font == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Cannot render '%s' as the font is null.", text.c_str());
      return;
   }

   if (text.empty()) {
      return;
   }

   SDL_Surface* surface =
       TTF_RenderText_Solid(font, text.c_str(), text.size(), color);
   if (!surface) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to create surface for text rendering: %s",
                   SDL_GetError());
      return;
   }

   SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
   if (!texture) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to create texture from surface for text "
                   "rendering: %s",
                   SDL_GetError());
      SDL_DestroySurface(surface);
      return;
   }

   auto const width = static_cast<float>(surface->w);
   float const leftEdge = align == TextAlign::Center  ? position.x() - width / 2
                          : align == TextAlign::Right ? position.x() - width
                                                      : position.x();

   SDL_FRect const textRect{
       .x = leftEdge,
       .y = position.y(),
       .w = width,
       .h = static_cast<float>(surface->h),
   };

   SDL_RenderTexture(renderer, texture, nullptr, &textRect);

   SDL_DestroyTexture(texture);
   SDL_DestroySurface(surface);
}

}  // namespace TextHelpers

}  // namespace YerbEngine
