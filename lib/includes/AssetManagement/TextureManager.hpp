#pragma once
#include <SDL.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace YerbEngine {
    class TextureManager {
        SDL_Renderer                                  *m_renderer;
        std::unordered_map<std::string, SDL_Texture *> m_textures{};

      public:
        explicit TextureManager(SDL_Renderer *renderer);
        ~TextureManager();

        void         registerTexture(std::string_view             name,
                                     std::filesystem::path const &path);
        bool         hasTexture(std::string_view name) const;
        SDL_Texture *getTexture(std::string_view name) const;
    };
} // namespace YerbEngine
