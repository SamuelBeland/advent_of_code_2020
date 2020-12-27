#include <algorithm>
#include <numeric>
#include <unordered_map>

#include "utils.hpp"
#include <resources.hpp>

using number_t = int;
using Numbers_And_Last_Sightings = std::unordered_map<number_t, number_t>;

//==============================================================================
auto parse_starting_numbers(std::string_view const & string)
{
    std::vector<number_t> starting_numbers{};
    scan_list(string, starting_numbers, ",");
    return starting_numbers;
}

//==============================================================================
auto play_game(std::vector<number_t> const & starting_numbers)
{
    static constexpr int TARGET_TURN = 2020;

    Numbers_And_Last_Sightings numbers{};
    int current_turn{ 1 };
    auto last_number{ starting_numbers.front() };
    for (auto it{ starting_numbers.cbegin() + 1 }; it != starting_numbers.cend(); ++it) {
        numbers[last_number] = current_turn++;
        last_number = *it;
    }

    for (; current_turn < TARGET_TURN; ++current_turn) {
        auto const find_result{ numbers.find(last_number) };
        if (find_result != numbers.cend()) {
            // number was found
            auto const turn_difference{ current_turn - find_result->second };
            last_number = turn_difference;
            find_result->second = current_turn;
        } else {
            // starting number
            numbers.emplace(std::make_pair(last_number, current_turn));
            last_number = 0;
        }
    }

    return last_number;
}

//==============================================================================
std::string day_15_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const starting_numbers{ parse_starting_numbers(input) };
    auto const result{ play_game(starting_numbers) };

    return std::to_string(result);
}

//==============================================================================
std::string day_15_b(char const * input_file_path)
{
    return "not implemented";
}