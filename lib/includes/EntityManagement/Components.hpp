#pragma once

#include <SDL.h>
#include <algorithm>
#include <string_view>
#include <vector>
#include <bitset>

#include <Helpers/Vec2.hpp>
namespace YerbEngine {
    namespace Components {
        class CTransform {
          public:
            Vec2 topLeftCornerPos{0, 0};
            Vec2 velocity{0, 0};

            CTransform(Vec2 const &position,
                       Vec2 const &velocity)
                : topLeftCornerPos(position),
                  velocity(velocity) {}

            CTransform() = default;
        };

        class CShape {

          public:
            SDL_Rect  rect;
            SDL_Color color;

            CShape(SDL_Rect  rect,
                   SDL_Color color)
                : rect(rect),
                  color(color) {}
        };

        class CInput {
          public:
            enum Directions { Forward, Backward, Left, Right };
            std::bitset<4> directions;
        };

        class CLifespan {
          public:
            Uint64 birthTime;
            Uint64 lifespan = 0;

            CLifespan() : birthTime(SDL_GetTicks64()) {}

            explicit CLifespan(Uint64 const lifespan)
                : birthTime(SDL_GetTicks64()),
                  lifespan(lifespan) {}
        };

        enum EffectTypes { Speed, Slowness };

        struct Effect {
            Uint64      startTime;
            Uint64      duration;
            EffectTypes type;
        };

        class CEffects {
            std::vector<Effect> effects;

          public:
            CEffects() = default;

            void addEffect(Effect const &effect) {
                for (auto const &[startTime, duration, type] : effects) {
                    if (type == effect.type) {
                        return;
                    }
                }

                effects.push_back(effect);
            }

            std::vector<Effect> const &getEffects() const { return effects; }

            void removeEffect(EffectTypes const type) {
                std::erase_if(effects, [type](Effect const &effect) -> bool {
                    return effect.type == type;
                });
            }

            bool hasEffect(EffectTypes const type) const {
                return std::ranges::find_if(
                           effects, [type](Effect const &effect) -> bool {
                               return effect.type == type;
                           }) != effects.end();
            }

            void clearEffects() { effects.clear(); }
        };

        class CBounceTracker {
            int m_bounces = 0;

          public:
            CBounceTracker() = default;

            void addBounce() { m_bounces++; }

            int getBounces() const { return m_bounces; }
        };

        class CSprite {
          public:
            explicit CSprite(std::string_view textureId)
                : m_textureId(textureId) {}

            std::string_view getTextureId() const { return m_textureId; }

          private:
            std::string_view m_textureId;
        };
    } // namespace Components
} // namespace YerbEngine
