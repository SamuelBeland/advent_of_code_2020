#include <algorithm>
#include <numeric>

#include "utils.hpp"
#include <resources.hpp>

using id_t = int;

//==============================================================================
id_t get_id(std::string_view const & seat)
{
    assert(seat.size() == 10);

    id_t result{};
    std::for_each(seat.cbegin(), seat.cend(), [&result](char const character) {
        result <<= 1;
        if (character == 'B' || character == 'R') {
            ++result;
        };
    });

    return result;
}

//==============================================================================
std::string day_5_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    std::vector<id_t> ids{};
    ids.resize(lines.size());
    std::transform(lines.cbegin(), lines.cend(), ids.begin(), get_id);
    auto const max_id{ *std::max_element(ids.cbegin(), ids.cend()) };

    return std::to_string(max_id);
}

//==============================================================================
std::string day_5_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    std::vector<id_t> ids{};
    ids.resize(lines.size());
    std::transform(lines.cbegin(), lines.cend(), ids.begin(), get_id);
    std::sort(ids.begin(), ids.end());

    auto it{ ids.cbegin() + 1 };
    auto last_it{ ids.cbegin() };
    for (; it < ids.end(); ++it, ++last_it) {
        if (*it - *last_it > 1) {
            return std::to_string(*last_it + 1);
        }
    }
    assert(false);
    return {};
}