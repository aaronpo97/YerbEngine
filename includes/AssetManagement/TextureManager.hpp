#pragma once
#include <SDL2/SDL.h>
#include <filesystem>
#include <unordered_map>

enum class TextureName { DEFAULT, PLAYER, WALL, COIN , ENEMY};
const std::unordered_map<TextureName, std::filesystem::path> imagePaths = {
    {TextureName::PLAYER, "assets/images/player.png"},
    {TextureName::WALL, "assets/images/wall.png"},
    {TextureName::COIN, "assets/images/coin.png"},
    {TextureName::ENEMY, "assets/images/enemy.png"}
};

class TextureManager {
  std::unordered_map<TextureName, SDL_Surface *> m_surfaces = {};
  std::unordered_map<TextureName, SDL_Texture *> m_textures = {};
  SDL_Renderer                                  *m_renderer;

  void loadTexture(TextureName name);

public:
  explicit TextureManager(SDL_Renderer *renderer);
  ~TextureManager();

  SDL_Texture *getTexture(TextureName name);
};
