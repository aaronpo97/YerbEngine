#include <AssetManagement/TextureManager.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <string_view>

namespace YerbEngine {

    TextureManager::TextureManager(SDL_Renderer *renderer)
        : m_renderer(renderer) {
        if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == -1) {
            std::cout << "SDL_image could not initialize! SDL_image Error: %s\n"
                      << IMG_GetError();
        }

        SDL_Log("TextureManager created.");
    }

    TextureManager::~TextureManager() {
        std::cout << "ImageManager destroyed\n";
        for (auto &[name, texture] : m_textures) {
            SDL_DestroyTexture(texture);
            SDL_Log("Texture %s freed", name.c_str());
        }
    }

    void TextureManager::registerTexture(std::string_view             name,
                                         std::filesystem::path const &path) {
        if (name.empty()) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                        "Attempted to register a texture with an empty name.");
            return;
        }

        std::string key{name};

        if (m_textures.contains(key)) {
            SDL_Log("Texture %.*s already registered, skipping.",
                    static_cast<int>(name.size()), name.data());
            return;
        }

        SDL_Surface *img = IMG_Load(path.c_str());

        if (img == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Unable to load image %s! SDL_image Error: %s",
                         path.c_str(), IMG_GetError());
            return;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, img);
        SDL_FreeSurface(img);

        if (texture == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Unable to create texture from %s! SDL Error: %s",
                         path.c_str(), SDL_GetError());
            return;
        }

        m_textures.emplace(std::move(key), texture);
    }

    bool TextureManager::hasTexture(std::string_view name) const {
        return m_textures.contains(std::string{name});
    }

    SDL_Texture *TextureManager::getTexture(std::string_view name) const {
        std::string key{name};
        auto const  it = m_textures.find(key);
        if (it == m_textures.end()) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                        "Texture %.*s not found in TextureManager.",
                        static_cast<int>(name.size()), name.data());
            return nullptr;
        }

        return it->second;
    }

} // namespace YerbEngine
