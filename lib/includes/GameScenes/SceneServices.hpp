#pragma once

#include <AssetManagement/AudioSampleBuffer.hpp>
#include <AssetManagement/FontManager.hpp>
#include <AssetManagement/TextureManager.hpp>
#include <GameScenes/ISceneRouter.hpp>
#include <SystemManagement/AudioManager.hpp>
#include <SystemManagement/IClock.hpp>
#include <SystemManagement/VideoManager.hpp>

namespace YerbEngine {

/**
 * What the engine lends a scene for as long as the scene is alive.
 *
 * A scene is handed this instead of the engine itself, so its constructor
 * states what it can touch. A game that adds its own collaborators (its
 * configuration, say) extends this and passes the larger bundle to its own
 * scenes.
 *
 * Every member is a reference to something the composition root owns and
 * outlives the scenes by construction.
 */
struct SceneServices {
   VideoManager& video;
   FontManager& fonts;
   TextureManager& textures;

   /** Queued one-shot samples, spread over frames by priority. */
   AudioSampleBuffer& audio;

   /** The mixer itself, for the looping background track and muting. */
   AudioManager& mixer;

   IClock const& clock;
   ISceneRouter& router;
};

}  // namespace YerbEngine
