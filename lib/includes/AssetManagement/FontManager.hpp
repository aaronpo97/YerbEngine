#pragma once

#include <SDL3_ttf/SDL_ttf.h>

#include <filesystem>

typedef std::filesystem::path Path;
namespace YerbEngine {

/** The three sizes every font is opened at. */
struct FontSizes {
   int sm = 38;
   int md = 48;
   int lg = 68;
};

class FontManager {
   Path m_fontPath;

   FontSizes m_sizes;

   TTF_Font* m_fontLg = nullptr;
   TTF_Font* m_fontMd = nullptr;
   TTF_Font* m_fontSm = nullptr;

  public:
   ~FontManager();

   FontManager(Path const& fontPath, FontSizes sizes);
   void loadFonts(Path const& fontPath);
   TTF_Font* getFontLg() const;
   TTF_Font* getFontMd() const;
   TTF_Font* getFontSm() const;
};
}  // namespace YerbEngine