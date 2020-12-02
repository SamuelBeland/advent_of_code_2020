#define CATCH_CONFIG_MAIN
#include <catch2>

#include <resources/resources.hpp>

TEST_CASE("day 1 a")
{
    REQUIRE(day_1_a(inputs::TEST_1_A_1) == "514579");
    REQUIRE(day_1_a(inputs::DAY_1) == "921504");

#ifdef NDEBUG
    BENCHMARK("day 1 a") { day_1_a(inputs::DAY_1); };
#endif
}

TEST_CASE("day 1 b")
{
    REQUIRE(day_1_b(inputs::TEST_1_A_1) == "241861950");
    REQUIRE(day_1_b(inputs::DAY_1) == "195700142");

#ifdef NDEBUG
    BENCHMARK("day 1 b") { day_1_b(inputs::DAY_1); };
#endif
}

TEST_CASE("day 2 a")
{
    REQUIRE(day_2_a(inputs::TEST_2_A_1) == "2");
    REQUIRE(day_2_a(inputs::DAY_2) == "467");

#ifdef NDEBUG
    BENCHMARK("day 2 a") { day_2_a(inputs::DAY_2); };
#endif
}

TEST_CASE("day 2 b")
{
    REQUIRE(day_2_b(inputs::TEST_2_A_1) == "1");
    REQUIRE(day_2_b(inputs::DAY_2) == "441");

#ifdef NDEBUG
    BENCHMARK("day 2 b") { day_2_b(inputs::DAY_2); };
#endif
}