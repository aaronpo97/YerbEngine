#pragma once

#include "./ComponentPool.hpp"
#include "./Components.hpp"

#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace YerbEngine {

    namespace detail {
        template <class>
        struct always_false : std::false_type {};
    } // namespace detail

    class ComponentRegistry {

        struct IPool {
            virtual ~IPool()               = default;
            virtual void remove(size_t id) = 0;
        };

        template <typename T>
        struct Pool final : IPool {
            ComponentPool<std::shared_ptr<T>> data;
            void remove(size_t id) override { data.remove(id); }
        };

        std::unordered_map<std::type_index, std::unique_ptr<IPool>> m_pools;

        template <typename T>
        Pool<T> &pool() {
            auto id = std::type_index(typeid(T));
            auto it = m_pools.find(id);
            if (it == m_pools.end()) {
                auto  storage = std::make_unique<Pool<T>>();
                auto *ptr     = storage.get();
                m_pools.emplace(id, std::move(storage));
                return *ptr;
            }
            return *static_cast<Pool<T> *>(it->second.get());
        }

        template <typename T>
        Pool<T> const *poolIfExists() const {
            auto it = m_pools.find(std::type_index(typeid(T)));
            if (it == m_pools.end()) {
                return nullptr;
            }
            return static_cast<Pool<T> const *>(it->second.get());
        }

      public:
        ComponentRegistry()  = default;
        ~ComponentRegistry() = default;

        template <typename T>
        std::shared_ptr<T> emplace(size_t             id,
                                   std::shared_ptr<T> component) {
            return pool<T>().data.emplace(id, std::move(component));
        }

        template <typename T>
        std::shared_ptr<T> get(size_t id) {
            auto *ptr = pool<T>().data.get(id);
            return ptr ? *ptr : nullptr;
        }

        template <typename T>
        std::shared_ptr<T> get(size_t id) const {
            auto const *poolPtr = poolIfExists<T>();
            if (!poolPtr) {
                return nullptr;
            }
            auto const *ptr = poolPtr->data.get(id);
            return ptr ? *ptr : nullptr;
        }

        template <typename T>
        bool contains(size_t id) const {
            auto const *poolPtr = poolIfExists<T>();
            return poolPtr ? poolPtr->data.contains(id) : false;
        }

        template <typename T>
        void remove(size_t id) {
            auto *poolPtr = const_cast<Pool<T> *>(poolIfExists<T>());
            if (poolPtr) {
                poolPtr->data.remove(id);
            }
        }

        template <typename T>
        std::vector<std::shared_ptr<T>> &dense() {
            return pool<T>().data.dense();
        }

        template <typename T>
        std::vector<std::shared_ptr<T>> const &dense() const {
            static std::vector<std::shared_ptr<T>> const empty;
            auto const *poolPtr = poolIfExists<T>();
            return poolPtr ? poolPtr->data.dense() : empty;
        }

        template <typename T>
        std::vector<size_t> &denseIds() {
            return pool<T>().data.denseIds();
        }

        template <typename T>
        std::vector<size_t> const &denseIds() const {
            static std::vector<size_t> const empty;
            auto const                      *poolPtr = poolIfExists<T>();
            return poolPtr ? poolPtr->data.denseIds() : empty;
        }

        void removeAllForEntity(size_t id) {
            for (auto &entry : m_pools) {
                entry.second->remove(id);
            }
        }
    };

} // namespace YerbEngine
