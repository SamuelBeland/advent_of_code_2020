//-- - Day 9: Encoding Error-- -
// With your neighbor happily enjoying their video game, you turn your attention to an open data port on the little
// screen in the seat in front of you.
//
// Though the port is non - standard, you manage to connect it to your computer through the clever use of several
// paperclips.Upon connection, the port outputs a series of numbers(your puzzle input).
//
// The data appears to be encrypted with the eXchange - Masking Addition System(XMAS) which, conveniently for you, is an
// old cypher with an important weakness.
//
// XMAS starts by transmitting a preamble of 25 numbers.After that, each number you receive should be the sum of any two
// of the 25 immediately previous numbers.The two numbers will have different values, and there might be more than one
// such pair.
//
// For example, suppose your preamble consists of the numbers 1 through 25 in a random order.To be valid, the next
// number must be the sum of two of those numbers :
//
// 26 would be a valid next number, as it could be 1 plus 25 (or many other pairs, like 2 and 24).
// 49 would be a valid next number, as it is the sum of 24 and 25.
// 100 would not be valid; no two of the previous 25 numbers sum to 100.
// 50 would also not be valid; although 25 appears in the previous 25 numbers, the two numbers in the pair must be
// different.
//
// Suppose the 26th number is 45, and the first number(no longer an option, as it is more than 25 numbers ago) was 20.
// Now, for the next number to be valid, there needs to be some pair of numbers among 1 - 19, 21 - 25, or 45 that add up
// to it :
//
// 26 would still be a valid next number, as 1 and 25 are still within the previous 25 numbers.
// 65 would not be valid, as no two of the available numbers sum to it.
// 64 and 66 would both be valid, as they are the result of 19 + 45 and 21 + 45 respectively.
//
// Here is a larger example which only considers the previous 5 numbers(and has a preamble of length 5) :
//
//    35
//    20
//    15
//    25
//    47
//    40
//    62
//    55
//    65
//    95
//    102
//    117
//    150
//    182
//    127
//    219
//    299
//    277
//    309
//    576
//
//    In this example, after the 5 - number preamble, almost every number is the sum of two of the previous 5 numbers;
//    the only number that does not follow this rule is 127.
//
//    The first step of attacking the weakness in the XMAS data is to find the first number in the list(after the
//    preamble) which is not the sum of two of the 25 numbers before it.What is the first number that does not have this
//    property ?
//
//    -- - Part Two-- -
//    The final step in breaking the XMAS encryption relies on the invalid number you just found : you must find a
//    contiguous set of at least two numbers in your list which sum to the invalid number from step 1.
//
//    Again consider the above example :
//
// 35
// 20
// 15
// 25
// 47
// 40
// 62
// 55
// 65
// 95
// 102
// 117
// 150
// 182
// 127
// 219
// 299
// 277
// 309
// 576
//
// In this list, adding up all of the numbers from 15 through 40 produces the invalid number from step 1, 127. (Of
// course, the contiguous set of numbers in your actual list might be much longer.)
//
// To find the encryption weakness, add together the smallestand largest number in this contiguous range; in this
// example, these are 15 and 47, producing 62.
//
// What is the encryption weakness in your XMAS - encrypted list of numbers ?

#include <type_traits>

#include "utils.hpp"
#include <resources.hpp>

namespace
{
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
number_t find_intruder(std::vector<number_t> const & numbers, size_t const preamble_size)
{
    auto preamble_begin{ numbers.cbegin() };
    auto preamble_end{ numbers.cbegin() + preamble_size };

    std::vector<number_t> preamble{};
    for (auto number_it{ preamble_end }; number_it != numbers.cend(); ++preamble_begin, ++preamble_end, ++number_it) {
        preamble.clear();
        preamble.assign(preamble_begin, preamble_end);
        sort(preamble);

        if (!is_sum_of_two_numbers_in_preamble(*number_it, preamble.cbegin(), preamble.cend())) {
            return *number_it;
        }
    }
    assert(false);
    return {};
}

//==============================================================================
number_t find_weakness(number_t const intruder, std::vector<number_t> const & numbers)
{
    // TODO : This is probably broken with some inputs since the numbers are not perfectly sorted.

    enum class Direction { up, down };

    auto small{ numbers.cbegin() };
    auto big{ small + 1 };
    auto direction{ Direction::up };
    auto sum{ *small + *big };

    while (true) {
        if (sum < intruder) {
            if (direction == Direction::up) {
                sum += *++big;
            } else {
                sum -= *small++;
                direction = Direction::up;
            }
        } else if (sum > intruder) {
            if (direction == Direction::down) {
                sum -= *big--;
            } else {
                sum -= *small++;
                direction = Direction::down;
            }
        } else {
            auto const smallest{ *std::min_element(small, big + 1) };
            auto const biggest{ *std::max_element(small, big + 1) };
            auto const result{ smallest + biggest };
            return result;
        }
    }
}

} // namespace

//==============================================================================
std::string day_9_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    auto const preamble_size{ parse_preamble_size(lines.front()) };
    auto const numbers{ parse_number_list<number_t>(lines.cbegin() + 1, lines.cend()) };
    auto const intruder{ find_intruder(numbers, preamble_size) };

    return std::to_string(intruder);
}

//==============================================================================
std::string day_9_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    auto const preamble_size{ parse_preamble_size(lines.front()) };
    auto const numbers{ parse_number_list<number_t>(lines.cbegin() + 1, lines.cend()) };
    auto const intruder{ find_intruder(numbers, preamble_size) };

    auto const weakness{ find_weakness(intruder, numbers) };
    return std::to_string(weakness);
}
