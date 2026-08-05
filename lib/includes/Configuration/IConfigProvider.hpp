#pragma once
#include "ConfigDictionary.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

namespace YerbEngine {
    class IConfigProvider {
        std::filesystem::path m_configPath;

      public:
        virtual ~IConfigProvider() = default;

        virtual ConfigValue getKey(std::string const &name) const = 0;
        virtual bool        hasKey(std::string const &name) const = 0;
    };

    class JsonConfigProvider : public IConfigProvider {
        ConfigDictionary m_configDict;

        void loadJsonIntoDict(nlohmann::json const &json,
                              std::string const    &prefix);

      public:
        explicit JsonConfigProvider(std::filesystem::path configPath);
        ConfigValue getKey(std::string const &name) const override;
        bool        hasKey(std::string const &name) const override;
    };
} // namespace YerbEngine
