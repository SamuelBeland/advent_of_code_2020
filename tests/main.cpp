#define CATCH_CONFIG_MAIN
#include <catch2>

#include <resources/resources.hpp>

TEST_CASE("day 1 a")
{
    REQUIRE(day_1_a(inputs::TEST_1_A_1) == "514579");

    REQUIRE(day_1_a(inputs::DAY_1) == "921504");
}