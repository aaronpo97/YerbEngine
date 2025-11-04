#pragma once
#include <SDL.h>
#include <string>
#include <unordered_map>
#include <variant>

using ConfigValue = std::variant<int, float, std::string, bool, SDL_Color>;

namespace YerbEngine {
    class ConfigDictionary {
        std::unordered_map<std::string, ConfigValue> m_configuration;

      public:
        ConfigDictionary()                                    = default;
        ~ConfigDictionary()                                   = default;
        ConfigDictionary(ConfigDictionary const &)            = delete;
        ConfigDictionary &operator=(ConfigDictionary const &) = delete;
        ConfigDictionary(ConfigDictionary &&)                 = delete;
        ConfigDictionary &operator=(ConfigDictionary &&)      = delete;

        template <typename T>
        void set(std::string const &key,
                 T                  value) {
            m_configuration[key] = value;
        }

        template <typename T>
        std::optional<T> get(std::string const &key) const {
            auto const it = m_configuration.find(key);
            if (it == m_configuration.end())
                return std::nullopt;
            if (!std::holds_alternative<T>(it->second))
                return std::nullopt;
            return std::get<T>(it->second);
        }

        bool contains(std::string const &key) const {
            return m_configuration.contains(key);
        }

        template <typename T>
        bool hasType(std::string const &key) const {
            auto const it = m_configuration.find(key);
            return it != m_configuration.end() &&
                   std::holds_alternative<T>(it->second);
        }

        void clear() { m_configuration.clear(); }
    };
} // namespace YerbEngine