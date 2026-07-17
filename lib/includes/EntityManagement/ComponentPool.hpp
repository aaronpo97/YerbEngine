#pragma once

#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

namespace YerbEngine {

    template <typename T>
    class ComponentPool {
        static constexpr size_t npos = std::numeric_limits<size_t>::max();

        std::vector<size_t> m_sparse;   // entity id -> dense index (or npos)
        std::vector<size_t> m_denseIds; // dense index -> entity id
        std::vector<T>      m_dense;    // dense index -> component data

      public:
        ComponentPool() = default;
        bool contains(size_t id) const {
            return id < m_sparse.size() && m_sparse[id] != npos;
        }

        size_t size() const { return m_dense.size(); }
        bool   empty() const { return m_dense.empty(); }

        void clear() {
            m_sparse.clear();
            m_denseIds.clear();
            m_dense.clear();
        }

        void resizeSparse(size_t size) {
            if (size > m_sparse.size()) {
                m_sparse.resize(size, npos);
            }
        }

        void reserveDense(size_t capacity) {
            m_dense.reserve(capacity);
            m_denseIds.reserve(capacity);
        }

        template <typename... Args>
        T &emplace(size_t id, Args &&...args) {
            if (id >= m_sparse.size()) {
                m_sparse.resize(id + 1, npos);
            }

            if (!contains(id)) {
                m_sparse[id] = m_dense.size();
                m_denseIds.push_back(id);
                m_dense.emplace_back(std::forward<Args>(args)...);
            }

            return m_dense[m_sparse[id]];
        }

        void remove(size_t id) {
            if (!contains(id)) {
                return;
            }

            size_t const idx  = m_sparse[id];
            size_t const last = m_dense.size() - 1;

            if (idx != last) {
                m_dense[idx]   = std::move(m_dense[last]);
                size_t movedId = m_denseIds[last];
                m_denseIds[idx] = movedId;
                m_sparse[movedId] = idx;
            }

            m_dense.pop_back();
            m_denseIds.pop_back();
            m_sparse[id] = npos;
        }

        T       *get(size_t id) { return contains(id) ? &m_dense[m_sparse[id]] : nullptr; }
        T const *get(size_t id) const {
            return contains(id) ? &m_dense[m_sparse[id]] : nullptr;
        }

        std::vector<T>      &dense() { return m_dense; }
        std::vector<T> const &dense() const { return m_dense; }
        std::vector<size_t>      &denseIds() { return m_denseIds; }
        std::vector<size_t> const &denseIds() const { return m_denseIds; }
    };

} // namespace YerbEngine
