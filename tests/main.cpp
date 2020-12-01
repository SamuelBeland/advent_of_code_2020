#define CATCH_CONFIG_MAIN
#include <catch2>

#include <resources/resources.hpp>

#include "../src/utils.hpp"

// TODO : fix this
constexpr char const * DAY_1_a_1 = "inputs/tests/1_a_1.txt";

TEST_CASE("day 1 a")
{
    REQUIRE(day_1_a(read_file(DAY_1_a_1).c_str()) == "514579");

    REQUIRE(day_1_a(read_file(DAYS_ALL[0].default_input_file_path).c_str()) == "921504");
}