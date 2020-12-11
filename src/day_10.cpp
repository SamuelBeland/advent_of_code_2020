#include <numeric>

#include "utils.hpp"
#include <resources.hpp>

using number_t = size_t;

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
std::vector<number_t> compute_differences(std::vector<number_t> const & numbers)
{
    std::vector<number_t> differences{};
    differences.resize(numbers.size() - 1);

    std::transform(numbers.cbegin() + 1, numbers.cend(), numbers.cbegin(), differences.begin(), std::minus());
    return differences;
}

//==============================================================================
std::string day_10_a(char const * input_file_path)
{
    auto const numbers{ get_day_10_numbers(input_file_path) };
    auto const differences{ compute_differences(numbers) };

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
constexpr number_t fucked_up_fibo(number_t const n)
{
    if (n == 0) {
        return 0;
    }
    if (n == 1 || n == 2) {
        return 1;
    }
    return fucked_up_fibo(n - 1) + fucked_up_fibo(n - 2) + fucked_up_fibo(n - 3);
}

//==============================================================================
template<number_t N>
constexpr std::array<number_t, N> get_pre_computed_fucked_up_fibo()
{
    std::array<number_t, N> result{};
    for (number_t i{}; i < N; ++i) {
        result[i] = fucked_up_fibo(i);
    }
    return result;
}

//==============================================================================
std::string day_10_b(char const * input_file_path)
{
    auto const numbers{ get_day_10_numbers(input_file_path) };
    auto const differences{ compute_differences(numbers) };

    std::vector<number_t> consecutives_ones{};

    size_t counter{};
    for (auto const diff : differences) {
        if (diff == 1) {
            ++counter;
        } else {
            if (counter > 1) {
                consecutives_ones.push_back(counter + 1); // diffs always remove one item
            }
            counter = 0;
        }
    }

    static constexpr auto FUCKED_UP_FIBO{ get_pre_computed_fucked_up_fibo<6>() };
    number_t count{ 1 };
    for (auto const & strike : consecutives_ones) {
        assert(strike != 0);
        count *= FUCKED_UP_FIBO[strike];
    }

    return std::to_string(count);
}
