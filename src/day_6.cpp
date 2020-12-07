#include "utils.hpp"
#include <resources.hpp>

//==============================================================================
int get_group_sum(std::string_view const & group)
{
    auto const person{ split(group) };
    return 0;
}

//==============================================================================
std::string day_6_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const groups{ split(input, "\n\n") };

    std::vector<int> group_sums{};
    group_sums.resize(groups.size());

    return "day_6_a not implemented";
}

//==============================================================================
std::string day_6_b(char const * input_file_path)
{
    return "day_6_b not implemented";
}
