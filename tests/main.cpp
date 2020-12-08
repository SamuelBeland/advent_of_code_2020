#define CATCH_CONFIG_MAIN
#include <catch2.hpp>

#include <resources.hpp>

//==============================================================================
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

//==============================================================================
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

//==============================================================================
TEST_CASE("day_3_a")
{
    REQUIRE(day_3_a(inputs::TEST_3_A_1) == "7");
    REQUIRE(day_3_a(inputs::DAY_3) == "265");
}

TEST_CASE("day_3_b")
{
    REQUIRE(day_3_b(inputs::TEST_3_A_1) == "336");
    REQUIRE(day_3_b(inputs::DAY_3) == "3154761400");
}

//==============================================================================
TEST_CASE("day_4_a")
{
    REQUIRE(day_4_a(inputs::TEST_4_A_1) == "2");
    REQUIRE(day_4_a(inputs::DAY_4) == "264");
}

TEST_CASE("day_4_b")
{
    REQUIRE(day_4_b(inputs::TEST_4_B_1) == "0");
    REQUIRE(day_4_b(inputs::TEST_4_B_2) == "4");
    REQUIRE(day_4_b(inputs::DAY_4) == "224");
}

//==============================================================================
TEST_CASE("day_5_a")
{
    REQUIRE(day_5_a(inputs::TEST_5_A_1) == "0");
    REQUIRE(day_5_a(inputs::TEST_5_A_2) == "1023");
    REQUIRE(day_5_a(inputs::TEST_5_A_3) == "119");
    REQUIRE(day_5_a(inputs::TEST_5_A_4) == "567");
    REQUIRE(day_5_a(inputs::TEST_5_A_5) == "820");
    REQUIRE(day_5_a(inputs::DAY_5) == "861");
}

TEST_CASE("day_5_b")
{
    REQUIRE(day_5_b(inputs::DAY_5) == "633");
}

//==============================================================================
TEST_CASE("day_6_a")
{
    REQUIRE(day_6_a(inputs::TEST_6_A_1) == "11");
    REQUIRE(day_6_a(inputs::DAY_6) == "6735");
}

TEST_CASE("day_6_b")
{
    REQUIRE(day_6_b(inputs::TEST_6_A_1) == "6");
    REQUIRE(day_6_b(inputs::DAY_6) == "3221");
}

//==============================================================================
TEST_CASE("day_7_a")
{
    REQUIRE(day_7_a(inputs::TEST_7_A_1) == "4");
    REQUIRE(day_7_a(inputs::DAY_7) == "229");
}

TEST_CASE("day_7_b")
{
    REQUIRE(day_7_b(inputs::TEST_7_A_1) == "32");
    REQUIRE(day_7_b(inputs::TEST_7_B_1) == "126");
}

//==============================================================================
#ifdef NDEBUG
TEST_CASE("Benchmarks")
{
    for (auto const & day : DAYS) {
        BENCHMARK(day.name) { return day.function(); };
    }

    BENCHMARK("ALL")
    {
        std::string result;
        for (auto const & day : DAYS) {
            result = day.function();
        }
        return result;
    };
}
#endif
