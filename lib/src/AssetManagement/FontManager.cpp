#include <AssetManagement/FontManager.hpp>

namespace YerbEngine {

FontManager::FontManager(Path const& fontPath, FontSizes const sizes)
    : m_fontPath(fontPath), m_sizes(sizes) {
   if (!TTF_Init()) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to initialize SDL_ttf: %s",
                   SDL_GetError());
      return;
   }

   SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "SDL_ttf initialized successfully.");
   loadFonts(fontPath);
}

void FontManager::loadFonts(Path const& fontPath) {
   m_fontSm = TTF_OpenFont(fontPath.c_str(), m_sizes.sm);
   m_fontMd = TTF_OpenFont(fontPath.c_str(), m_sizes.md);
   m_fontLg = TTF_OpenFont(fontPath.c_str(), m_sizes.lg);

   bool const fontsLoaded =
       m_fontLg != nullptr && m_fontMd != nullptr && m_fontSm != nullptr;

   if (!fontsLoaded) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load fonts: %s",
                   SDL_GetError());
      return;
   }

   SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Fonts loaded successfully!");
}

TTF_Font* FontManager::getFontLg() const { return m_fontLg; }

TTF_Font* FontManager::getFontMd() const { return m_fontMd; }

TTF_Font* FontManager::getFontSm() const { return m_fontSm; }

FontManager::~FontManager() {
   SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Cleaning up fonts...");

   if (m_fontMd != nullptr) {
      TTF_CloseFont(m_fontMd);
      m_fontMd = nullptr;
   }

   if (m_fontSm != nullptr) {
      TTF_CloseFont(m_fontSm);
      m_fontSm = nullptr;
   }

   if (m_fontLg != nullptr) {
      TTF_CloseFont(m_fontLg);
      m_fontLg = nullptr;
   }

   TTF_Quit();

   SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Fonts cleaned up successfully!");
}

}  // namespace YerbEngine