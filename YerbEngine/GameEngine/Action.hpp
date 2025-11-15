#pragma once
#include <YerbEngine/Helpers/Vec2.hpp>
#include <optional>
#include <string>

enum ActionState { START, END };
class Action {
  private:
    std::string         m_name;  // Action name, e.g., "SHOOT", "JUMP"
    ActionState         m_state; // State of the action
    std::optional<Vec2> m_pos;

  public:
    Action(std::string                name,
           ActionState const         &state,
           std::optional<Vec2> const &pos);

    std::string const         &getName() const;
    ActionState const         &getState() const;
    std::optional<Vec2> const &getPos() const;
};
