#include <EntityManagement/Components.hpp>
#include <Systems/RenderSystem.hpp>

namespace YerbEngine {

namespace RenderSystem {

void clear(VideoManager& videoManager, SDL_Color const& color) {
   SDL_Renderer* renderer = videoManager.getRenderer();
   SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
   SDL_RenderClear(renderer);
}

void renderEntities(EntityManager& entityManager, VideoManager& videoManager,
                    TextureManager& textureManager) {
   SDL_Renderer* renderer = videoManager.getRenderer();

   for (auto const& entity : entityManager.getEntities()) {
      auto const& cShape = entity->getComponent<Components::CShape>();
      auto const& cTransform = entity->getComponent<Components::CTransform>();

      if (cShape == nullptr || cTransform == nullptr) {
         continue;
      }

      SDL_FRect& rect = cShape->rect;
      Vec2 const& position = cTransform->topLeftCornerPos;
      rect.x = position.x();
      rect.y = position.y();

      auto const& cSprite = entity->getComponent<Components::CSprite>();
      if (cSprite == nullptr) {
         SDL_SetRenderDrawColor(renderer, cShape->color.r, cShape->color.g,
                                cShape->color.b, cShape->color.a);
         SDL_RenderFillRect(renderer, &rect);
         continue;
      }

      SDL_Texture* texture = textureManager.getTexture(cSprite->getTextureId());
      SDL_RenderTexture(renderer, texture, nullptr, &rect);
   }
}

void present(VideoManager& videoManager) {
   SDL_RenderPresent(videoManager.getRenderer());
}

}  // namespace RenderSystem

}  // namespace YerbEngine
