#include <Configuration/AudioManifest.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace YerbEngine {

namespace {
nlohmann::json readJson(std::filesystem::path const& path) {
   std::ifstream file(path);
   if (!file.is_open()) {
      throw std::runtime_error("Failed to open audio manifest: " +
                               path.string());
   }

   try {
      nlohmann::json json;
      file >> json;
      return json;
   } catch (nlohmann::json::parse_error const& error) {
      throw std::runtime_error("Failed to parse audio manifest " +
                               path.string() + ": " + error.what());
   }
}
}  // namespace

AudioManifest loadAudioManifest(std::filesystem::path const& manifestPath,
                                std::filesystem::path const& assetsDir) {
   AudioManifest manifest;

   if (!std::filesystem::exists(manifestPath)) {
      SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                  "Audio manifest missing at %s; starting without audio.",
                  manifestPath.string().c_str());
      return manifest;
   }

   nlohmann::json const json = readJson(manifestPath);

   for (auto const& track : json.value("tracks", nlohmann::json::array())) {
      manifest.options.tracks.push_back({
          .id = track.at("id").get<AudioTrackId>(),
          .filepath = assetsDir / track.at("file").get<std::string>(),
      });
   }

   for (auto const& sample : json.value("samples", nlohmann::json::array())) {
      auto id = sample.at("id").get<AudioSampleId>();

      manifest.options.samples.push_back({
          .id = id,
          .filepath = assetsDir / sample.at("file").get<std::string>(),
          .defaultVolume =
              sample.value("volume", AudioManager::DEFAULT_SAMPLE_VOLUME),
      });

      if (sample.contains("cooldown")) {
         manifest.sampleCooldowns[std::move(id)] =
             sample.at("cooldown").get<Uint64>();
      }
   }

   SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO,
               "Loaded %zu tracks and %zu samples from %s",
               manifest.options.tracks.size(), manifest.options.samples.size(),
               manifestPath.string().c_str());

   return manifest;
}

}  // namespace YerbEngine
