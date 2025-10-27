#pragma once
#include <cmath>
#include <numbers>

namespace MathHelpers {
    inline constexpr float pi = std::numbers::pi_v<float>;

    constexpr float degreesToRadians(float const degrees) {
        return degrees * pi / 180.0f;
    }

    constexpr float radiansToDegrees(float const radians) {
        return radians * 180.0f / pi;
    }

    /**
     * @brief Version of the Pythagorean theorem that doesn't calculate the
     * square root.
     */
    constexpr float pythagorasSquared(float const a, float const b) {
        return a * a + b * b;
    }

    /**
     * @brief Pythagorean theorem, returns actual value.
     */
    constexpr float pythagoras(float const a, float const b) {
        return std::sqrt(pythagorasSquared(a, b));
    }

}; // namespace MathHelpers