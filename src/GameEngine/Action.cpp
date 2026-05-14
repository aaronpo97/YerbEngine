#include <utility>

#include <YerbEngine/GameEngine/Action.hpp>
#include <YerbEngine/Helpers/Vec2.hpp>

Action::Action(std::string const         &name,
               ActionState const         &state,
               std::optional<Vec2> const &pos)
    : m_name(name), m_state(state), m_pos(pos) {}

std::string const &Action::getName() const { return m_name; }

ActionState const &Action::getState() const { return m_state; }

std::optional<Vec2> const &Action::getPos() const { return m_pos; }
