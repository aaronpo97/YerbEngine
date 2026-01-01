#pragma once

#include <GameEngine/Action.hpp>
#include <utility>
#include <vector>

namespace YerbEngine {

    class ActionQueue {
      public:
        void push(Action action) { m_queue.push_back(std::move(action)); }
        std::vector<Action> drain() {
            std::vector<Action> out;
            out.swap(m_queue);
            return out;
        }
        bool empty() const { return m_queue.empty(); }

      private:
        std::vector<Action> m_queue;
    };

} // namespace YerbEngine
