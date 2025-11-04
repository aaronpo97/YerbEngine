#pragma once
#include "ConfigDictionary.hpp"
#include "IConfigProvider.hpp"
#include <memory>
#include <string>

namespace YerbEngine {
    class ConfigStore {
        std::unique_ptr<IConfigProvider> m_provider;
        ConfigDictionary                 m_dictionary;

    public:
        explicit ConfigStore(std::unique_ptr<IConfigProvider> provider);

        static std::unique_ptr<ConfigStore>
        fromJsonFile(std::filesystem::path const &jsonPath);

        ConfigValue get(std::string const &path) const;
        bool        has(std::string const &path) const;
        void        reload();
    };
} // namespace YerbEngine