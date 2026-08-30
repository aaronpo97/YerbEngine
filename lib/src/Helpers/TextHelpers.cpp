#include <Helpers/TextHelpers.hpp>

namespace YerbEngine {

namespace TextHelpers {
void renderLineOfText(SDL_Renderer* renderer, TTF_Font* font,
                      std::string const& text, SDL_Color const& color,
                      Vec2 const& position) {
   // Render the text to a surface
   SDL_Surface* surface =
       TTF_RenderText_Solid(font, text.c_str(), text.size(), color);
   if (!surface) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to create surface for text rendering: %s",
                   SDL_GetError());
      return;  // Exit if the surface creation fails
   }

   // Create a texture from the surface
   SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
   if (!texture) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to create texture from surface for text "
                   "rendering: %s",
                   SDL_GetError());
      SDL_DestroySurface(
          surface);  // Free the surface if texture creation fails
      return;        // Exit if the texture creation fails
   }

   // Set up the rectangle for the text
   SDL_FRect textRect;
   textRect.x = position.x();
   textRect.y = position.y();
   textRect.w = static_cast<float>(surface->w);
   textRect.h = static_cast<float>(surface->h);

   // Copy the texture to the renderer
   SDL_RenderTexture(renderer, texture, nullptr, &textRect);

   SDL_DestroyTexture(texture);
   SDL_DestroySurface(surface);
}

}  // namespace TextHelpers

}  // namespace YerbEngine