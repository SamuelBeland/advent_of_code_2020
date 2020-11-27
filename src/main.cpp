#include <catch_with_main.hpp>
#include <fmt/format.h>
#include <scn/scn.h>

// int main(int argc, char const ** argv)
//{
//	fmt::print("hello\n");
//
//	return 0;
//}

TEST_CASE("hello")
{
    std::string a{ "foo" };
    int i;
    REQUIRE(scn::scan(a, "{}", i));
}
