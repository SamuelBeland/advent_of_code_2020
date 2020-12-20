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

struct Num_Info {
    uint64_t number;
    uint64_t distance_from_last_number;
};

std::vector<Num_Info> parse_num_infos(std::string_view const & line)
{
    uint64_t distance_from_last_number{};
    auto const elements{ split(line, ",") };

    std::vector<Num_Info> result{};

    auto const parse_element = [&result, &distance_from_last_number](std::string_view const & element) {
        if (element.front() != 'x') {
            uint64_t number;
            [[maybe_unused]] auto const from_chars_result{
                std::from_chars(element.data(), element.data() + element.size(), number)
            };
            assert(from_chars_result.ec == std::errc());
            result.push_back(Num_Info{ number, distance_from_last_number });
            distance_from_last_number = 1;
        }
    };

    parse_element(elements.front());
    for (auto element{ elements.cbegin() + 1 }; element < elements.cend(); ++element) {
        ++distance_from_last_number;
        parse_element(*element);
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

uint64_t find_number(uint64_t const big, uint64_t const difference, uint64_t const start, uint64_t const increment)
{
    auto cur{ start };
    while (cur % big != difference) {
        cur += increment;
    }
    return cur;
}

//==============================================================================
std::string day_13_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };
    assert(lines.size() == 2);

    auto const num_infos{ parse_num_infos(lines.back()) };
    uint64_t start{};
    auto increment{ num_infos[0].number };

    for (auto num_info{ num_infos.cbegin() }; num_info != num_infos.cend(); ++num_info) {
        start = find_number(num_info->number, num_info->distance_from_last_number, start, increment);
        increment *= num_info->number;
    }

    return std::to_string(start);
}