#include "utils.hpp"
#include <resources.hpp>

using number_t = int;

//==============================================================================
auto get_day_10_numbers(char const * const input_file_path)
{
    static constexpr number_t BUILTIN_ADAPTER_RATING_DIFFERENCE = 3;

    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };
    auto numbers{ parse_number_list<number_t>(lines) };
    std::sort(numbers.begin(), numbers.end());
    numbers.insert(numbers.begin(), 0);
    numbers.push_back(numbers.back() + BUILTIN_ADAPTER_RATING_DIFFERENCE);
    return numbers;
}

//==============================================================================
std::string day_10_a(char const * input_file_path)
{
    auto const numbers{ get_day_10_numbers(input_file_path) };
    std::vector<number_t> differences{};
    differences.resize(numbers.size() - 1);

    std::transform(numbers.cbegin() + 1, numbers.cend(), numbers.cbegin(), differences.begin(), std::minus());
    size_t diff_by_one{};
    size_t diff_by_three{};

    auto const increase_counters = [&diff_by_one, &diff_by_three](number_t const number) {
        if (number == 1) {
            ++diff_by_one;
        } else if (number == 3) {
            ++diff_by_three;
        }
    };
    std::for_each(differences.cbegin(), differences.cend(), increase_counters);

    auto const result{ diff_by_one * diff_by_three };
    return std::to_string(result);
}

//==============================================================================
std::string day_10_b(char const * input_file_path)
{
    return "day_10_b not implemented";
}
