#include <algorithm>

#include "utils.hpp"

std::string day_1_a(const char * input_file_path)
{
    static constexpr auto TARGET_VALUE = 2020;

    auto const input{ read_file(input_file_path) };
    auto const words{ split(input, '\n') };
    auto numbers{ convert_words<int>(words) };

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

std::string day_1_b(const char * input_file_path)
{
    return "coucou";
}