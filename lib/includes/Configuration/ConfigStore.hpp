#pragma once
#include "ConfigDictionary.hpp"
#include "IConfigProvider.hpp"
#include <Helpers/Vec2.hpp>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace YerbEngine {
    class ConfigStore {
        std::unique_ptr<IConfigProvider>        m_provider;
        std::map<std::string, ConfigDictionary> m_dictionary;

      public:
        explicit ConfigStore(std::unique_ptr<IConfigProvider> provider);

        static std::unique_ptr<ConfigStore>
        fromJsonFile(std::filesystem::path const &jsonPath);

        ConfigValue get(std::string const &path) const;
        void        reload();
    };
} // namespace YerbEngine
