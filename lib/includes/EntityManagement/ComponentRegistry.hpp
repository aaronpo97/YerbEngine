#pragma once

#include "./ComponentPool.hpp"
#include "./Components.hpp"

#include <memory>
#include <type_traits>

namespace YerbEngine {

    namespace detail {
        template <class>
        struct always_false : std::false_type {};
    } // namespace detail

    class ComponentRegistry {
        ComponentPool<std::shared_ptr<CTransform>>     m_transforms;
        ComponentPool<std::shared_ptr<CShape>>         m_shapes;
        ComponentPool<std::shared_ptr<CInput>>         m_inputs;
        ComponentPool<std::shared_ptr<CLifespan>>      m_lifespans;
        ComponentPool<std::shared_ptr<CEffects>>       m_effects;
        ComponentPool<std::shared_ptr<CBounceTracker>> m_bounceTrackers;
        ComponentPool<std::shared_ptr<CSprite>>        m_sprites;

        template <typename T>
        auto &pool() {
            if constexpr (std::is_same_v<T, CTransform>) {
                return m_transforms;
            } else if constexpr (std::is_same_v<T, CShape>) {
                return m_shapes;
            } else if constexpr (std::is_same_v<T, CInput>) {
                return m_inputs;
            } else if constexpr (std::is_same_v<T, CLifespan>) {
                return m_lifespans;
            } else if constexpr (std::is_same_v<T, CEffects>) {
                return m_effects;
            } else if constexpr (std::is_same_v<T, CBounceTracker>) {
                return m_bounceTrackers;
            } else if constexpr (std::is_same_v<T, CSprite>) {
                return m_sprites;
            } else {
                static_assert(detail::always_false<T>::value,
                              "Unsupported component type");
            }
        }

        template <typename T>
        auto const &pool() const {
            if constexpr (std::is_same_v<T, CTransform>) {
                return m_transforms;
            } else if constexpr (std::is_same_v<T, CShape>) {
                return m_shapes;
            } else if constexpr (std::is_same_v<T, CInput>) {
                return m_inputs;
            } else if constexpr (std::is_same_v<T, CLifespan>) {
                return m_lifespans;
            } else if constexpr (std::is_same_v<T, CEffects>) {
                return m_effects;
            } else if constexpr (std::is_same_v<T, CBounceTracker>) {
                return m_bounceTrackers;
            } else if constexpr (std::is_same_v<T, CSprite>) {
                return m_sprites;
            } else {
                static_assert(detail::always_false<T>::value,
                              "Unsupported component type");
            }
        }

      public:
        ComponentRegistry()  = default;
        ~ComponentRegistry() = default;

        template <typename T>
        std::shared_ptr<T> emplace(size_t id,
                                   std::shared_ptr<T> component) {
            return pool<T>().emplace(id, std::move(component));
        }

        template <typename T>
        std::shared_ptr<T> get(size_t id) {
            auto *ptr = pool<T>().get(id);
            return ptr ? *ptr : nullptr;
        }

        template <typename T>
        std::shared_ptr<T> get(size_t id) const {
            auto const *ptr = pool<T>().get(id);
            return ptr ? *ptr : nullptr;
        }

        template <typename T>
        bool contains(size_t id) const {
            return pool<T>().contains(id);
        }

        template <typename T>
        void remove(size_t id) {
            pool<T>().remove(id);
        }

        template <typename T>
        std::vector<std::shared_ptr<T>> &dense() {
            return pool<T>().dense();
        }

        template <typename T>
        std::vector<std::shared_ptr<T>> const &dense() const {
            return pool<T>().dense();
        }

        template <typename T>
        std::vector<size_t> &denseIds() {
            return pool<T>().denseIds();
        }

        template <typename T>
        std::vector<size_t> const &denseIds() const {
            return pool<T>().denseIds();
        }

        void removeAllForEntity(size_t id) {
            m_transforms.remove(id);
            m_shapes.remove(id);
            m_inputs.remove(id);
            m_lifespans.remove(id);
            m_effects.remove(id);
            m_bounceTrackers.remove(id);
            m_sprites.remove(id);
        }
    };

} // namespace YerbEngine
