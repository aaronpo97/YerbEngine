#include <Helpers/Vec2.hpp>

std::string Vec2::toString() const {
    return "Vec2(" + std::to_string(_x) + ", " + std::to_string(_y) + ")";
}

std::ostream &operator<<(std::ostream &os, Vec2 const &vec2) {
    os << vec2.toString();
    return os;
}