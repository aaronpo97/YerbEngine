#pragma once

#include <SDL.h>
#include <algorithm>
#include <vector>

#include <Configuration/Config.hpp>
#include <Helpers/Vec2.hpp>

class CTransform {
  public:
    Vec2 topLeftCornerPos{0, 0};
    Vec2 velocity{0, 0};

    CTransform(Vec2 const &position, Vec2 const &velocity)
        : topLeftCornerPos(position), velocity(velocity) {}

    CTransform() = default;
};

class CShape {
    SDL_Renderer *renderer;

  public:
    SDL_Rect  rect;
    SDL_Color color;

    CShape(SDL_Renderer *renderer, ShapeConfig const &config)
        : renderer(renderer), rect(), color() {

        if (renderer == nullptr) {
            SDL_LogError(
                SDL_LOG_CATEGORY_SYSTEM,
                "CShape entity initialization failed: Renderer is null.");
            throw std::runtime_error(
                "CShape entity initialization failed: Renderer is null.");
        }

        rect.h = static_cast<int>(config.height);
        rect.w = static_cast<int>(config.width);
        color  = config.color;

        SDL_SetRenderDrawColor(renderer, config.color.r, config.color.g,
                               config.color.b, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

class CInput {
  public:
    bool forward  = false;
    bool backward = false;
    bool left     = false;
    bool right    = false;

    CInput() = default;
};

class CLifespan {
  public:
    Uint64 birthTime;
    Uint64 lifespan = 0;

    CLifespan() : birthTime(SDL_GetTicks64()) {}

    explicit CLifespan(Uint64 const lifespan)
        : birthTime(SDL_GetTicks64()), lifespan(lifespan) {}
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
        return std::ranges::find_if(effects,
                                    [type](Effect const &effect) -> bool {
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
    SDL_Texture *m_texture;

  public:
    explicit CSprite(SDL_Texture *texture) : m_texture(texture) {}

    SDL_Texture *getTexture() const { return m_texture; }
};