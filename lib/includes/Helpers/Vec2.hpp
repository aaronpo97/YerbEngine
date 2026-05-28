#pragma once
#include <Helpers/MathHelpers.hpp>
#include <iostream>

namespace YerbEngine {

    class Vec2 {
    float _x, _y;

  public:
    constexpr explicit Vec2(float const x = 0, float const y = 0)
        : _x(x), _y(y) {}

    constexpr bool operator==(Vec2 const &rhs) const {
        return rhs._x == _x && rhs._y == _y;
    }

    constexpr bool operator!=(Vec2 const &rhs) const { return !(*this == rhs); }

    constexpr Vec2 operator-(Vec2 const &rhs) const {

        return Vec2(_x - rhs._x, _y - rhs._y);
    }

    constexpr Vec2 operator+(Vec2 const &rhs) const {
        return Vec2(_x + rhs._x, _y + rhs._y);
    }

    constexpr Vec2 operator*(float const val) const {
        return Vec2(_x * val, _y * val);
    }

    constexpr Vec2 operator/(float const val) const {
        return Vec2(_x / val, _y / val);
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

    constexpr Vec2 normalize() {
        float len = MathHelpers::pythagoras(_x, _y);
        if (len > 0) {
            _x /= len;
            _y /= len;
        }
        return *this;
    }

    constexpr float x() const { return _x; }
    constexpr float y() const { return _y; }

    constexpr void setX(float const x) { _x = x; }
    constexpr void setY(float const y) { _y = y; }

    constexpr float euclideanDistance(Vec2 const &other) const {
        return MathHelpers::pythagoras(other._x - _x, other._y - _y);
    }

    constexpr float euclideanDistanceSquared(Vec2 const &other) const {
        return MathHelpers::pythagorasSquared(other._x - _x, other._y - _y);
    }

    std::string toString() const;
    };

} // namespace YerbEngine