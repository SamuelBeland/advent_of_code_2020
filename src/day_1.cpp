#include <algorithm>

#include "utils.hpp"

static constexpr auto TARGET_VALUE = 2020;

std::string day_1_a(const char * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const words{ split(input, '\n') };
    auto numbers{ convert_words<int>(words) };

    std::sort(numbers.begin(), numbers.end());

    auto small{ numbers.cbegin() };
    auto big{ numbers.cend() - 1 };

    auto sum{ *small + *big };

    while (sum != TARGET_VALUE) {
        if (*small + *big < TARGET_VALUE) {
            ++small;
        } else {
            --big;
        }
        assert(small < big);
        sum = *small + *big;
    }

    return std::to_string(*small * *big);
}

std::string day_1_b(const char * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const words{ split(input, '\n') };
    auto numbers{ convert_words<int>(words) };

    std::sort(numbers.begin(), numbers.end());

    auto small{ numbers.cbegin() };
    auto middle{ small + 1 };
    auto big{ numbers.cend() - 1 };

    auto sum{ *small + *middle + *big };

    while (sum != TARGET_VALUE) {
        if (sum > TARGET_VALUE) {
            --big;
            small = numbers.cbegin();
            middle = small + 1;
        } else {
            while (sum < TARGET_VALUE && (middle + 1) < big) {
                ++middle;
                assert((small < middle) && (middle < big));
                sum = *small + *middle + *big;
            }
            if (sum != TARGET_VALUE) {
                ++small;
                middle = small + 1;
            }
        }
        assert(small < middle && middle < big);
        sum = *small + *middle + *big;
    }

    return std::to_string(*small * *middle * *big);
}