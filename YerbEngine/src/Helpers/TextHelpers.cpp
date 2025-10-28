#include <Helpers/TextHelpers.hpp>

namespace TextHelpers {
    void renderLineOfText(SDL_Renderer      *renderer,
                          TTF_Font          *font,
                          std::string const &text,
                          SDL_Color const   &color,
                          Vec2 const        &position) {

        // Render the text to a surface
        SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
        if (!surface) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                         "Failed to create surface for text rendering: %s",
                         TTF_GetError());
            return; // Exit if the surface creation fails
        }

        // Create a texture from the surface
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                         "Failed to create texture from surface for text "
                         "rendering: %s",
                         SDL_GetError());
            SDL_FreeSurface(
                surface); // Free the surface if texture creation fails
            return;       // Exit if the texture creation fails
        }

        // Set up the rectangle for the text
        SDL_Rect textRect;
        textRect.x = static_cast<int>(position.x());
        textRect.y = static_cast<int>(position.y());
        textRect.w = surface->w;
        textRect.h = surface->h;

        // Copy the texture to the renderer
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
} // namespace TextHelpers