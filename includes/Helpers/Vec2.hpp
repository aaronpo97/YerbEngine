#pragma once
#include <iostream>
class Vec2 {
  public:
    float x, y;

    Vec2(float x = 0, float y = 0);

    bool operator==(Vec2 const &rhs) const;
    bool operator!=(Vec2 const &rhs) const;

    Vec2 operator-(Vec2 const &rhs) const;
    Vec2 operator+(Vec2 const &rhs) const;
    Vec2 operator*(float const val) const;
    Vec2 operator/(float const val) const;

    void operator+=(Vec2 const &rhs);
    void operator-=(Vec2 const &rhs);
    void operator*=(float const val);
    void operator/=(float const val);

    friend std::ostream &operator<<(std::ostream &os, Vec2 const &vec2);

    Vec2  normalize();
    float length() const;
};
