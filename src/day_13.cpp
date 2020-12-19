#include <numeric>

#include "utils.hpp"
#include <resources.hpp>

std::vector<int> parse_bus_ids(std::string_view const & line)
{
    assert(line.size() > 0);

    std::vector<int> result{};
    auto const * cur{ line.data() };
    auto const * const end{ line.data() + line.size() };

    while (cur < end) {
        auto const * next{ std::find(cur, end, ',') };
        if (*cur != 'x') {
            int new_id;
            [[maybe_unused]] auto const from_chars_result{ std::from_chars(cur, next, new_id) };
            assert(from_chars_result.ec == std::errc());
            result.push_back(new_id);
        }
        cur = next + 1;
    }

    return result;
}

//==============================================================================
std::string day_13_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };
    assert(lines.size() == 2);

    int depart_time;
    scan(lines.front(), "{}", depart_time);

    auto const bus_ids{ parse_bus_ids(lines.back()) };

    int min_wait_time{ depart_time };
    int min_id{};

    for (auto const id : bus_ids) {
        auto const wait_time{ id - depart_time % id };
        if (wait_time < min_wait_time) {
            min_wait_time = wait_time;
            min_id = id;
        }
    }

    auto const result{ min_id * min_wait_time };
    return std::to_string(result);
}

//==============================================================================
std::string day_13_b(char const * input_file_path)
{
    return "day_13_b not implemented";
}