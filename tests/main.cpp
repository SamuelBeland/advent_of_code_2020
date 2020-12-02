#define CATCH_CONFIG_MAIN
#include <catch2>

#include <resources/resources.hpp>

TEST_CASE("day_1_a")
{
    REQUIRE(day_1_a(inputs::TEST_1_A_1) == "514579");
    REQUIRE(day_1_a(inputs::DAY_1) == "921504");
}

TEST_CASE("day_1_b")
{
    REQUIRE(day_1_b(inputs::TEST_1_A_1) == "241861950");
    REQUIRE(day_1_b(inputs::DAY_1) == "195700142");
}

TEST_CASE("day_2_a")
{
    REQUIRE(day_2_a(inputs::TEST_2_A_1) == "2");
    REQUIRE(day_2_a(inputs::DAY_2) == "467");
}

TEST_CASE("day_2_b")
{
    REQUIRE(day_2_b(inputs::TEST_2_A_1) == "1");
    REQUIRE(day_2_b(inputs::DAY_2) == "441");
}

#ifdef NDEBUG
TEST_CASE("Benchmarks")
{
    for (auto const & day : DAYS) {
        BENCHMARK(day.name) { return day.function(); };
    }
}
#endif