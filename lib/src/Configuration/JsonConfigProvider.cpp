#include "Configuration/IConfigProvider.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace YerbEngine {
    JsonConfigProvider::JsonConfigProvider(std::filesystem::path configPath) {
        if (!std::filesystem::exists(configPath)) {
            throw std::runtime_error("Config file not found: " +
                                     configPath.string());
        }

        std::ifstream file(configPath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open config file: " +
                                     configPath.string());
        }

        nlohmann::json json;
        try {
            file >> json;
        } catch (nlohmann::json::parse_error const &e) {
            throw std::runtime_error("Failed to parse JSON: " +
                                     std::string(e.what()));
        }

        // Load JSON values into ConfigDictionary
        loadJsonIntoDict(json, "");
    }

    ConfigValue JsonConfigProvider::getKey(std::string const &name) const {
        // Try each type in the variant
        if (auto val = m_configDict.get<int>(name))
            return *val;
        if (auto val = m_configDict.get<float>(name))
            return *val;
        if (auto val = m_configDict.get<std::string>(name))
            return *val;
        if (auto val = m_configDict.get<bool>(name))
            return *val;
        if (auto val = m_configDict.get<SDL_Color>(name))
            return *val;

        // Return default value if not found
        return 0;
    }

    bool JsonConfigProvider::hasKey(std::string const &name) const {
        return m_configDict.contains(name);
    }

    void JsonConfigProvider::loadJsonIntoDict(nlohmann::json const &json,
                                              std::string const    &prefix) {
        for (auto const &[key, value] : json.items()) {
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;

            if (value.is_object()) {
                // Check if it's an SDL_Color object
                if (value.contains("r") && value.contains("g") &&
                    value.contains("b")) {
                    SDL_Color color;
                    color.r = value.value("r", 0);
                    color.g = value.value("g", 0);
                    color.b = value.value("b", 0);
                    color.a = value.value("a", 255);
                    m_configDict.set<SDL_Color>(fullKey, color);
                } else {
                    // Recursively load nested objects
                    loadJsonIntoDict(value, fullKey);
                }
            } else if (value.is_number_integer()) {
                m_configDict.set<int>(fullKey, value.get<int>());
            } else if (value.is_number_float()) {
                m_configDict.set<float>(fullKey, value.get<float>());
            } else if (value.is_string()) {
                m_configDict.set<std::string>(fullKey,
                                              value.get<std::string>());
            } else if (value.is_boolean()) {
                m_configDict.set<bool>(fullKey, value.get<bool>());
            }
        }
    }
} // namespace YerbEngine
