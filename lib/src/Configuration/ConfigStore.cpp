#include "Configuration/ConfigStore.hpp"
#include "Configuration/IConfigProvider.hpp"

namespace YerbEngine {
    ConfigStore::ConfigStore(std::unique_ptr<IConfigProvider> provider)
        : m_provider(std::move(provider))
    {
        if (!m_provider) {
            throw std::runtime_error(
                "ConfigStore requires a valid IConfigProvider");
        }
    }

    std::unique_ptr<ConfigStore>
    ConfigStore::fromJsonFile(std::filesystem::path const &jsonPath)
    {
        auto provider = std::make_unique<JsonConfigProvider>(jsonPath);
        return std::make_unique<ConfigStore>(std::move(provider));
    }

    ConfigValue ConfigStore::get(std::string const &path) const
    {
        return m_provider->getKey(path);
    }

    bool ConfigStore::has(std::string const &path) const
    {
        return m_provider->hasKey(path);
    }

    void ConfigStore::reload()
    {
        // Clear cached dictionaries if you add caching later
        m_dictionary.clear();

        // For now, the provider handles all the data
        // Future: could implement caching or lazy loading here
    }
} // namespace YerbEngine
