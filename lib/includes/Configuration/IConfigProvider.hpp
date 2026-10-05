#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

#include "ConfigDictionary.hpp"

namespace YerbEngine {
class IConfigProvider {
   std::filesystem::path m_configPath;

  public:
   virtual ~IConfigProvider() = default;

   virtual ConfigValue getKey(std::string const& name) const = 0;
   virtual bool hasKey(std::string const& name) const = 0;
};

/**
 * A provider holding no keys, so every read falls back to its default.
 *
 * Lets a game that ships no configuration file use the same code path as
 * one that does.
 */
class EmptyConfigProvider final : public IConfigProvider {
  public:
   ConfigValue getKey(std::string const&) const override { return 0; }
   bool hasKey(std::string const&) const override { return false; }
};

class JsonConfigProvider : public IConfigProvider {
   ConfigDictionary m_configDict;

   void loadJsonIntoDict(nlohmann::json const& json, std::string const& prefix);

  public:
   explicit JsonConfigProvider(std::filesystem::path configPath);
   ConfigValue getKey(std::string const& name) const override;
   bool hasKey(std::string const& name) const override;
};
}  // namespace YerbEngine
