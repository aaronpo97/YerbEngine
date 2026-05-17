#pragma once
#include <YerbEngine/Helpers/MathHelpers.hpp>
#include <cmath>
#include <iostream>
class Vec2 {
  private:
    float _x, _y;

  public:
    constexpr Vec2(float x = 0, float y = 0) : _x(x), _y(y) {}

    constexpr bool operator==(Vec2 const &rhs) const {
        return rhs._x == _x && rhs._y == _y;
    }
    constexpr bool operator!=(Vec2 const &rhs) const { return !(*this == rhs); }

    constexpr Vec2 operator-(Vec2 const &rhs) const {
        return {_x - rhs._x, _y - rhs._y};
    }
    constexpr Vec2 operator+(Vec2 const &rhs) const {
        return {_x + rhs._x, _y + rhs._y};
    }
    constexpr Vec2 operator*(float const val) const {
        return {_x * val, _y * val};
    }
    constexpr Vec2 operator/(float const val) const {
        return {_x / val, _y / val};
    }

    constexpr void operator+=(Vec2 const &rhs) {
        _x += rhs._x;
        _y += rhs._y;
    }
    constexpr void operator-=(Vec2 const &rhs) {
        _x -= rhs._x;
        _y -= rhs._y;
    }
    constexpr void operator*=(float const val) {
        _x *= val;
        _y *= val;
    }
    constexpr void operator/=(float const val) {
        _x /= val;
        _y /= val;
    }

    friend std::ostream &operator<<(std::ostream &os, Vec2 const &vec2);

    // the normalize function will make the vector a unit vector (length of 1)
    // pointing in the same direction
    constexpr Vec2 normalize() {
        float len = MathHelpers::pythagoras(_x, _y);
        if (len > 0) { // Avoid division by zero
            _x /= len;
            _y /= len;
        }
        return *this;
    }

    constexpr float x() const { return _x; }
    constexpr float y() const { return _y; }

    constexpr void setX(float const x) { _x = x; }
    constexpr void setY(float const y) { _y = y; }

    std::string toString() const;
};
