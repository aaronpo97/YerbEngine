#pragma once
#include <SDL3/SDL.h>

#include <Configuration/ConfigStore.hpp>
#include <Helpers/Vec2.hpp>
#include <string>

namespace YerbEngine {

/**
 * Reads typed values out of a ConfigStore, falling back to a supplied
 * default whenever the key is absent or holds another type.
 */
class ConfigReader {
   ConfigStore& m_store;

  public:
   explicit ConfigReader(ConfigStore& store) : m_store(store) {}

   ConfigStore& store() const { return m_store; }

   float floatOr(std::string const& key, float fallback) const;
   int intOr(std::string const& key, int fallback) const;
   Uint64 u64Or(std::string const& key, Uint64 fallback) const;
   std::string strOr(std::string const& key, std::string fallback) const;
   SDL_Color colorOr(std::string const& key, SDL_Color fallback) const;

   /** Reads "<base>.width" and "<base>.height" into a Vec2. */
   Vec2 vec2Or(std::string const& base, Vec2 fallback) const;
};

}  // namespace YerbEngine
