#pragma once

#include <memory>
#include <string>

namespace YerbEngine {

class Scene;

/**
 * How a scene hands control onward.
 *
 * A scene builds its successor and names it; the router decides what that
 * means. Scenes pass a constructed scene rather than a type or a tag so a
 * successor can take whatever runtime arguments it needs (a match mode, a
 * final score) without the router knowing about them.
 */
class ISceneRouter {
  public:
   virtual ~ISceneRouter() = default;

   /**
    * Makes a scene the active one, under the given name.
    *
    * @param sceneName The name to register the scene under.
    * @param scene The scene to activate.
    */
   virtual void LoadScene(std::string const& sceneName,
                          std::shared_ptr<Scene> const& scene) = 0;

   /** Ends the run. */
   virtual void quit() = 0;
};

}  // namespace YerbEngine
