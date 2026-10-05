#pragma once
#include <SDL3/SDL.h>

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace YerbEngine {

/**
 * A renderer the holder borrows rather than owns.
 *
 * Wrapping the pointer gives it a type of its own, which is what lets it be
 * passed as a dependency rather than fetched from whoever created it.
 */
struct RendererHandle {
   SDL_Renderer* renderer = nullptr;
};

class TextureManager {
   SDL_Renderer* m_renderer;
   std::unordered_map<std::string, SDL_Texture*> m_textures{};

  public:
   explicit TextureManager(RendererHandle renderer);
   ~TextureManager();

   void registerTexture(std::string_view name,
                        std::filesystem::path const& path);
   bool hasTexture(std::string_view name) const;
   SDL_Texture* getTexture(std::string_view name) const;
};
}  // namespace YerbEngine
