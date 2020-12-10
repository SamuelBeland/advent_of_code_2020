#include <type_traits>

#include "utils.hpp"
#include <resources.hpp>

using number_t = size_t;

//==============================================================================
size_t parse_preamble_size(std::string_view const & line)
{
    size_t preamble;
    scan(line, "preamble: {}", preamble);
    return preamble;
}

//==============================================================================
template<typename It>
bool is_sum_of_two_numbers_in_preamble(number_t const & number, It const preamble_begin, It const preamble_end)
{
    static_assert(
        std::is_same<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>::value);
    assert(std::is_sorted(preamble_begin, preamble_end));

    auto small{ preamble_begin };
    auto big{ preamble_end - 1 };

    while (small < big) {
        auto const sum{ *small + *big };
        if (sum < number) {
            ++small;
        } else if (sum > number) {
            --big;
        } else {
            return true;
        }
    }

    return false;
}

//==============================================================================
std::string day_9_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    auto const preamble_size{ parse_preamble_size(lines.front()) };
    auto const numbers{ parse_number_list<number_t>(lines.cbegin() + 1, lines.cend()) };

    auto preamble_begin{ numbers.cbegin() };
    auto preamble_end{ numbers.cbegin() + preamble_size };

    std::vector<number_t> preamble{};
    for (auto number_it{ preamble_end }; number_it != numbers.cend(); ++preamble_begin, ++preamble_end, ++number_it) {
        preamble.clear();
        preamble.assign(preamble_begin, preamble_end);
        std::sort(preamble.begin(), preamble.end());

        if (!is_sum_of_two_numbers_in_preamble(*number_it, preamble.cbegin(), preamble.cend())) {
            return std::to_string(*number_it);
        }
    }
    assert(false);
    return {};
}

//==============================================================================
std::string day_9_b(char const * input_file_path)
{
    return "day_9_b not implemented!";
}
