#include "../tests/Timer.hpp"
#include "Helpers/MathHelpers.hpp"
#include "Helpers/Vec2.hpp"
#include <boost/test/unit_test.hpp>

using namespace YerbEngine;

BOOST_AUTO_TEST_SUITE(Vec2Tests)

BOOST_AUTO_TEST_CASE(vec2_construction) {
    Timer t("Vec2 construction");

    Vec2 v1;
    BOOST_CHECK_CLOSE(v1.x(), 0.0f, 0.001f);
    BOOST_CHECK_CLOSE(v1.y(), 0.0f, 0.001f);

    Vec2 v2(3.0f, 4.0f);
    BOOST_CHECK_CLOSE(v2.x(), 3.0f, 0.001f);
    BOOST_CHECK_CLOSE(v2.y(), 4.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_equality) {
    Timer t("Vec2 equality");
    Vec2  v1(1.0f, 2.0f);
    Vec2  v2(1.0f, 2.0f);
    Vec2  v3(2.0f, 3.0f);

    BOOST_CHECK(v1 == v2);
    BOOST_CHECK(v1 != v3);
}

BOOST_AUTO_TEST_CASE(vec2_addition) {
    Timer t("Vec2 addition");
    Vec2  v1(1.0f, 2.0f);
    Vec2  v2(3.0f, 4.0f);
    Vec2  result = v1 + v2;

    BOOST_CHECK_CLOSE(result.x(), 4.0f, 0.001f);
    BOOST_CHECK_CLOSE(result.y(), 6.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_subtraction) {
    Timer t("Vec2 subtraction");
    Vec2  v1(5.0f, 7.0f);
    Vec2  v2(2.0f, 3.0f);
    Vec2  result = v1 - v2;

    BOOST_CHECK_CLOSE(result.x(), 3.0f, 0.001f);
    BOOST_CHECK_CLOSE(result.y(), 4.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_scalar_multiplication) {
    Timer t("Vec2 scalar multiplication");
    Vec2  v(2.0f, 3.0f);
    Vec2  result = v * 2.0f;

    BOOST_CHECK_CLOSE(result.x(), 4.0f, 0.001f);
    BOOST_CHECK_CLOSE(result.y(), 6.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_addition_assignment) {
    Timer t("Vec2 addition assignment");
    Vec2  v1(1.0f, 2.0f);
    Vec2  v2(3.0f, 4.0f);
    v1 += v2;

    BOOST_CHECK_CLOSE(v1.x(), 4.0f, 0.001f);
    BOOST_CHECK_CLOSE(v1.y(), 6.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_scalar_division) {
    Timer t("Vec2 scalar division");
    Vec2  v(4.0f, 6.0f);
    Vec2  result = v / 2.0f;

    BOOST_CHECK_CLOSE(result.x(), 2.0f, 0.001f);
    BOOST_CHECK_CLOSE(result.y(), 3.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_subtraction_assignment) {
    Timer t("Vec2 subtraction assignment");
    Vec2  v1(5.0f, 7.0f);
    Vec2  v2(2.0f, 3.0f);
    v1 -= v2;

    BOOST_CHECK_CLOSE(v1.x(), 3.0f, 0.001f);
    BOOST_CHECK_CLOSE(v1.y(), 4.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_multiplication_assignment) {
    Timer t("Vec2 multiplication assignment");
    Vec2  v(2.0f, 3.0f);
    v *= 2.0f;

    BOOST_CHECK_CLOSE(v.x(), 4.0f, 0.001f);
    BOOST_CHECK_CLOSE(v.y(), 6.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_division_assignment) {
    Timer t("Vec2 division assignment");
    Vec2  v(4.0f, 6.0f);
    v /= 2.0f;

    BOOST_CHECK_CLOSE(v.x(), 2.0f, 0.001f);
    BOOST_CHECK_CLOSE(v.y(), 3.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_normalize) {
    Timer t("Vec2 normalize");
    Vec2  v(3.0f, 4.0f);
    v.normalize();

    // Length should be 1
    float length = MathHelpers::pythagoras(v.x(), v.y());
    BOOST_CHECK_CLOSE(length, 1.0f, 0.001f);

    // Direction should be preserved (ratio)
    BOOST_CHECK_CLOSE(v.x() / v.y(), 3.0f / 4.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_normalize_zero_vector) {
    Timer t("Vec2 normalize zero vector");
    Vec2  v(0.0f, 0.0f);
    v.normalize();

    // Should remain zero
    BOOST_CHECK_CLOSE(v.x(), 0.0f, 0.001f);
    BOOST_CHECK_CLOSE(v.y(), 0.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_euclidean_distance) {
    Timer t("Vec2 euclidean distance");
    Vec2  v1(0.0f, 0.0f);
    Vec2  v2(3.0f, 4.0f);

    float distance = v1.euclideanDistance(v2);
    BOOST_CHECK_CLOSE(distance, 5.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_euclidean_distance_squared) {
    Timer t("Vec2 euclidean distance squared");
    Vec2  v1(0.0f, 0.0f);
    Vec2  v2(3.0f, 4.0f);

    float distanceSquared = v1.euclideanDistanceSquared(v2);
    BOOST_CHECK_CLOSE(distanceSquared, 25.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_setters) {
    Timer t("Vec2 setters");
    Vec2  v;
    v.setX(5.0f);
    v.setY(10.0f);

    BOOST_CHECK_CLOSE(v.x(), 5.0f, 0.001f);
    BOOST_CHECK_CLOSE(v.y(), 10.0f, 0.001f);
}

BOOST_AUTO_TEST_CASE(vec2_to_string) {
    Timer t("Vec2 toString");

    Vec2        v(1.5f, 2.5f);
    std::string str = v.toString();

    // Just verify it doesn't crash and returns something
    BOOST_CHECK(!str.empty());
    BOOST_CHECK(str.find("Vec2") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
