#include <YerbEngine/Helpers/Vec2.hpp>
#include <cmath>
#include <iostream>

Vec2::Vec2(float const x, float const y) : x(x), y(y) {}

bool Vec2::operator==(Vec2 const &rhs) const {
    return rhs.x == x && rhs.y == y;
}

bool Vec2::operator!=(Vec2 const &rhs) const { return !(*this == rhs); }

Vec2 Vec2::operator-(Vec2 const &rhs) const { return {x - rhs.x, y - rhs.y}; }

Vec2 Vec2::operator+(Vec2 const &rhs) const { return {x + rhs.x, y + rhs.y}; }

Vec2 Vec2::operator*(float const val) const { return {x * val, y * val}; }

Vec2 Vec2::operator/(float const val) const { return {x / val, y / val}; }

void Vec2::operator+=(Vec2 const &rhs) {
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator-=(Vec2 const &rhs) {
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator*=(float const val) {
    x *= val;
    y *= val;
}

void Vec2::operator/=(float const val) {
    x /= val;
    y /= val;
}

float Vec2::length() const { return std::sqrt(x * x + y * y); }

// the normalize function will make the vector a unit vector (length of 1)
// pointing in the same direction
Vec2 Vec2::normalize() {
    float const len = length();
    if (len > 0) { // Avoid division by zero
        x /= len;
        y /= len;
    }

    return *this;
}

std::ostream &operator<<(std::ostream &os, Vec2 const &vec2) {
    os << "Vec2(" << vec2.x << ", " << vec2.y << ")";
    return os;
}