#include <boost/test/unit_test.hpp>
#include "Helpers/MathHelpers.hpp"
#include <fstream>
#include "../tests/Timer.hpp"
using namespace YerbEngine;
BOOST_AUTO_TEST_SUITE(MathHelpersTests)
BOOST_AUTO_TEST_CASE(pythagorean_triples) {
    Timer t("Pythagorean triples");
    std::ifstream testDataFile("tests/data/PythagoreanTriple.csv");
    std::string line;
    std::getline(testDataFile, line); // Skip header line
    auto test = [](float a, float b, float expected) -> void {
        BOOST_CHECK_CLOSE(MathHelpers::pythagoras(a, b), expected, 1e-5f);
    };
    while(std::getline(testDataFile, line)) {
        std::istringstream ss(line);
        std::string token;
        // n,a1,a2,a3,p,q,perimeter,area,p/q,atan(p/q)/atan(1)
        // Skip n (column 0)
        std::getline(ss, token, ',');
        std::getline(ss, token, ',');
        float a1 = std::stof(token);
        std::getline(ss, token, ',');
        float a2 = std::stof(token);
        std::getline(ss, token, ',');
        float a3 = std::stof(token);
        test(a1, a2, a3);
    } 
    if (!testDataFile.is_open()) {
        BOOST_FAIL("Failed to open test data file: tests/data/PythagoreanTriple.csv");
    }
    auto result = 5.0f;
    BOOST_CHECK_CLOSE(result, 5.0f, 0.001f);
}
BOOST_AUTO_TEST_CASE(pythagoras_negative_values) {
    Timer t("Pythagoras negative values");
    float result = MathHelpers::pythagoras(-3.0f, -4.0f);
    BOOST_CHECK_CLOSE(result, 5.0f, 0.001f);
}
BOOST_AUTO_TEST_CASE(pythagoras_zero) {
    Timer t("Pythagoras zero values");
    float result = MathHelpers::pythagoras(0.0f, 0.0f);
    BOOST_CHECK_CLOSE(result, 0.0f, 0.001f);
}
BOOST_AUTO_TEST_CASE(degrees_to_radians_conversion) {
    Timer t("Degrees to radians conversion");
    float radians = MathHelpers::degreesToRadians(180.0f);
    BOOST_CHECK_CLOSE(radians, MathHelpers::pi, 0.001f);
}
BOOST_AUTO_TEST_CASE(radians_to_degrees_conversion) {
    Timer t("Radians to degrees conversion");
    float degrees = MathHelpers::radiansToDegrees(MathHelpers::pi);
    BOOST_CHECK_CLOSE(degrees, 180.0f, 0.001f);
}
BOOST_AUTO_TEST_SUITE_END()
