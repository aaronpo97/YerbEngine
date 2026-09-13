#include <Configuration/ConfigReader.hpp>

namespace YerbEngine {

float ConfigReader::floatOr(std::string const& key,
                            float const fallback) const {
   if (!m_store.has(key)) {
      return fallback;
   }

   ConfigValue const value = m_store.get(key);
   if (std::holds_alternative<float>(value)) {
      return std::get<float>(value);
   }
   if (std::holds_alternative<int>(value)) {
      return static_cast<float>(std::get<int>(value));
   }
   return fallback;
}

int ConfigReader::intOr(std::string const& key, int const fallback) const {
   if (!m_store.has(key)) {
      return fallback;
   }

   ConfigValue const value = m_store.get(key);
   return std::holds_alternative<int>(value) ? std::get<int>(value) : fallback;
}

Uint64 ConfigReader::u64Or(std::string const& key,
                           Uint64 const fallback) const {
   if (!m_store.has(key)) {
      return fallback;
   }

   ConfigValue const value = m_store.get(key);
   return std::holds_alternative<int>(value)
              ? static_cast<Uint64>(std::get<int>(value))
              : fallback;
}

std::string ConfigReader::strOr(std::string const& key,
                                std::string fallback) const {
   if (!m_store.has(key)) {
      return std::move(fallback);
   }

   ConfigValue const value = m_store.get(key);
   return std::holds_alternative<std::string>(value)
              ? std::get<std::string>(value)
              : std::move(fallback);
}

SDL_Color ConfigReader::colorOr(std::string const& key,
                                SDL_Color const fallback) const {
   if (!m_store.has(key)) {
      return fallback;
   }

   ConfigValue const value = m_store.get(key);
   return std::holds_alternative<SDL_Color>(value) ? std::get<SDL_Color>(value)
                                                   : fallback;
}

Vec2 ConfigReader::vec2Or(std::string const& base, Vec2 const fallback) const {
   return Vec2{floatOr(base + ".width", fallback.x()),
               floatOr(base + ".height", fallback.y())};
}

}  // namespace YerbEngine
