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
    REQUIRE(day_7_b(inputs::DAY_7) == "6683");
}

//==============================================================================
TEST_CASE("day_8_a")
{
    REQUIRE(day_8_a(inputs::TEST_8_A_1) == "5");
    REQUIRE(day_8_a(inputs::DAY_8) == "1671");
}

TEST_CASE("day_8_b")
{
    REQUIRE(day_8_b(inputs::TEST_8_A_1) == "8");
    REQUIRE(day_8_b(inputs::DAY_8) == "892");
}

//==============================================================================
TEST_CASE("day_9_a")
{
    REQUIRE(day_9_a(inputs::TEST_9_A_1) == "127");
    REQUIRE(day_9_a(inputs::DAY_9) == "21806024");
}

TEST_CASE("day_9_b")
{
    REQUIRE(day_9_b(inputs::TEST_9_A_1) == "62");
    REQUIRE(day_9_b(inputs::DAY_9) == "2986195");
}

//==============================================================================
TEST_CASE("day_10_a")
{
    REQUIRE(day_10_a(inputs::TEST_10_A_1) == "35");
    REQUIRE(day_10_a(inputs::TEST_10_A_2) == "220");
    REQUIRE(day_10_a(inputs::DAY_10) == "2244");
}

TEST_CASE("day_10_b")
{
    REQUIRE(day_10_b(inputs::TEST_10_A_1) == "8");
    REQUIRE(day_10_b(inputs::TEST_10_A_2) == "19208");
    REQUIRE(day_10_b(inputs::DAY_10) == "3947645370368");
}

//==============================================================================
TEST_CASE("day_11_a")
{
    REQUIRE(day_11_a(inputs::TEST_11_A_1) == "1");
    REQUIRE(day_11_a(inputs::TEST_11_A_2) == "4");
    REQUIRE(day_11_a(inputs::TEST_11_A_3) == "37");
    REQUIRE(day_11_a(inputs::DAY_11) == "2247");
}

TEST_CASE("day_11_b")
{
    REQUIRE(day_11_b(inputs::TEST_11_A_1) == "1");
    REQUIRE(day_11_b(inputs::TEST_11_A_2) == "4");
    REQUIRE(day_11_b(inputs::TEST_11_A_3) == "26");
    REQUIRE(day_11_b(inputs::DAY_11) == "2011");
}

//==============================================================================
TEST_CASE("day_12_a")
{
    REQUIRE(day_12_a(inputs::TEST_12_A_1) == "25");
    REQUIRE(day_12_a(inputs::DAY_12) == "381");
}

TEST_CASE("day_12_b")
{
    REQUIRE(day_12_b(inputs::TEST_12_A_1) == "286");
    REQUIRE(day_12_b(inputs::DAY_12) == "28591");
}

//==============================================================================
#ifdef NDEBUG
TEST_CASE("Benchmarks")
{
    for (auto day_it{ DAYS.crbegin() }; day_it != DAYS.crend(); ++day_it) {
        BENCHMARK(day_it->name) { return day_it->function(); };
    }

    BENCHMARK("ALL")
    {
        std::string result;
        for (auto const & day : DAYS) {
            result.append(day.function());
        }
        return result;
    };
}
#endif
