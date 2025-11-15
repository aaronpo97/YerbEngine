#include <YerbEngine/AssetManagement/TextureManager.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <ranges>

TextureManager::TextureManager(SDL_Renderer *renderer) : m_renderer(renderer) {
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == -1) {
        std::cout << "SDL_image could not initialize! SDL_image Error: %s\n"
                  << IMG_GetError();
    }

    for (auto const &name : imagePaths | std::views::keys) {
        loadTexture(name);
    }
    SDL_Log("TextureManager created.");
}

TextureManager::~TextureManager() {
    std::cout << "ImageManager destroyed\n";
    for (auto &[name, surface] : m_surfaces) {
        SDL_FreeSurface(surface);
        SDL_Log("Surface %d freed", name);
    }

    for (auto &[name, texture] : m_textures) {
        SDL_DestroyTexture(texture);
        SDL_Log("Texture %d freed", name);
    }
}

void TextureManager::loadTexture(TextureName const name) {
    std::filesystem::path const &path = imagePaths.at(name);
    SDL_Surface                 *img  = IMG_Load(path.c_str());

    if (img == nullptr) {
        std::cout << "Unable to load image! SDL_image Error:\n"
                  << path.c_str() << IMG_GetError();
    }

    m_surfaces[name] = img;
    m_textures[name] = SDL_CreateTextureFromSurface(m_renderer, img);
}

SDL_Texture *TextureManager::getTexture(TextureName const name) {
    if (std::ranges::find_if(imagePaths, [&](auto const &pair) {
            return pair.first == name;
        }) == imagePaths.end()) {
        std::cout << "Image not found in ImageManager\n";
        return nullptr;
    }

    return m_textures[name];
}
