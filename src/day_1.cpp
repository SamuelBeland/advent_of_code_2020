#include <algorithm>

#include <resources/resources.hpp>

#include "utils.hpp"

std::string day_1_a(char const * input)
{
    static constexpr auto TARGET_VALUE = 2020;

    std::string const input_string{ input };
    auto const words{ split(input_string) };

    auto numbers{ convert_list<int>(words) };

    std::sort(numbers.begin(), numbers.end());

    auto small{ numbers.cbegin() };
    auto big{ numbers.cend() - 1 };

    while (*small + *big != TARGET_VALUE) {
        if (*small + *big < TARGET_VALUE) {
            small += 1;
        } else {
            big -= 1;
        }
    }

    return std::to_string(*small * *big);
}