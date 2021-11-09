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
TEST_CASE("day_13_a")
{
    REQUIRE(day_13_a(inputs::TEST_13_A_1) == "295");
    REQUIRE(day_13_a(inputs::DAY_13) == "153");
}

TEST_CASE("day_13_b")
{
    REQUIRE(day_13_b(inputs::TEST_13_B_1) == "3417");
    REQUIRE(day_13_b(inputs::TEST_13_B_2) == "754018");
    REQUIRE(day_13_b(inputs::TEST_13_B_3) == "779210");
    REQUIRE(day_13_b(inputs::TEST_13_A_1) == "1068781");
    REQUIRE(day_13_b(inputs::TEST_13_B_4) == "1261476");
    REQUIRE(day_13_b(inputs::TEST_13_B_5) == "1202161486");
    REQUIRE(day_13_b(inputs::DAY_13) == "471793476184394");
}

//==============================================================================
TEST_CASE("day_14_a")
{
    REQUIRE(day_14_a(inputs::TEST_14_A_1) == "165");
    REQUIRE(day_14_a(inputs::DAY_14) == "8471403462063");
}

TEST_CASE("day_14_b")
{
    REQUIRE(day_14_b(inputs::TEST_14_B_1) == "208");
    REQUIRE(day_14_b(inputs::DAY_14) == "2667858637669");
}

//==============================================================================
TEST_CASE("day_15_a")
{
    REQUIRE(day_15_a(inputs::TEST_15_A_1) == "436");
    REQUIRE(day_15_a(inputs::TEST_15_A_2) == "1");
    REQUIRE(day_15_a(inputs::TEST_15_A_3) == "10");
    REQUIRE(day_15_a(inputs::TEST_15_A_4) == "27");
    REQUIRE(day_15_a(inputs::TEST_15_A_5) == "78");
    REQUIRE(day_15_a(inputs::TEST_15_A_6) == "438");
    REQUIRE(day_15_a(inputs::TEST_15_A_7) == "1836");
    REQUIRE(day_15_a(inputs::DAY_15) == "536");
}

TEST_CASE("day_15_b")
{
    // NOTE : these tests are way too expensive to run.

    // REQUIRE(day_15_b(inputs::TEST_15_A_1) == "175594");
    // REQUIRE(day_15_b(inputs::TEST_15_A_2) == "2578");
    // REQUIRE(day_15_b(inputs::TEST_15_A_3) == "3544142");
    // REQUIRE(day_15_b(inputs::TEST_15_A_4) == "261214");
    // REQUIRE(day_15_b(inputs::TEST_15_A_5) == "6895259");
    // REQUIRE(day_15_b(inputs::TEST_15_A_6) == "18");
    // REQUIRE(day_15_b(inputs::TEST_15_A_7) == "362");
    // REQUIRE(day_15_b(inputs::DAY_15) == "24065124");
}

//==============================================================================
TEST_CASE("day_16_a")
{
    REQUIRE(day_16_a(inputs::TEST_16_A_1) == "71");
    REQUIRE(day_16_a(inputs::DAY_16) == "19060");
}

TEST_CASE("day_16_b")
{
    REQUIRE(day_16_b(inputs::TEST_16_B_1) == "132");
    REQUIRE(day_16_b(inputs::TEST_16_B_2) == "110");
    REQUIRE(day_16_b(inputs::DAY_16) == "953713095011");
}

//==============================================================================
TEST_CASE("day_17_a")
{
    REQUIRE(day_17_a(inputs::TEST_17_A_1) == "0");
    REQUIRE(day_17_a(inputs::TEST_17_A_2) == "0");
    REQUIRE(day_17_a(inputs::TEST_17_A_3) == "112");
    REQUIRE(day_17_a(inputs::DAY_17) == "391");
}

TEST_CASE("day_17_b")
{
    REQUIRE(day_17_b(inputs::DAY_17) == "2264");
}

//==============================================================================
TEST_CASE("day_18_a")
{
    REQUIRE(day_18_a(inputs::TEST_18_A_1) == "1");
    REQUIRE(day_18_a(inputs::TEST_18_A_2) == "8");
    REQUIRE(day_18_a(inputs::TEST_18_A_3) == "71");
    REQUIRE(day_18_a(inputs::TEST_18_A_4) == "51");
    REQUIRE(day_18_a(inputs::TEST_18_A_5) == "26");
    REQUIRE(day_18_a(inputs::TEST_18_A_6) == "437");
    REQUIRE(day_18_a(inputs::TEST_18_A_7) == "12240");
    REQUIRE(day_18_a(inputs::TEST_18_A_8) == "13632");
    REQUIRE(day_18_a(inputs::DAY_18) == "9535936849815");
}

TEST_CASE("day_18_b")
{
    REQUIRE(day_18_b(inputs::TEST_18_B_1) == "51");
    REQUIRE(day_18_b(inputs::TEST_18_B_2) == "46");
    REQUIRE(day_18_b(inputs::TEST_18_B_3) == "1445");
    REQUIRE(day_18_b(inputs::TEST_18_B_4) == "669060");
    REQUIRE(day_18_b(inputs::TEST_18_B_5) == "23340");
    REQUIRE(day_18_b(inputs::DAY_18) == "472171581333710");
}

//==============================================================================
#ifdef NDEBUG
TEST_CASE("Benchmarks")
{
    for (auto day_it{ DAYS.crbegin() }; day_it != DAYS.crend(); ++day_it) {
        // TEMP : removing day_15_b because it's taking forever
        // if (strcmp(day_it->name, "day_15_b") == 0) {
        //    continue;
        // }

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
