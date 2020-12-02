//-- - Day 2: Password Philosophy-- -
// Your flight departs in a few days from the coastal airport; the easiest way down to the coast from here is via
// toboggan.
//
// The shopkeeper at the North Pole Toboggan Rental Shop is having a bad day. "Something's wrong with our computers; we
// can't log in!" You ask if you can take a look.
//
// Their password database seems to be a little corrupted : some of the passwords wouldn't have been allowed by the
// Official Toboggan Corporate Policy that was in effect when they were chosen.
//
// To try to debug the problem, they have created a list(your puzzle input) of passwords(according to the corrupted
// database) and the corporate policy when that password was set.
//
// For example, suppose you have the following list :
//
// 1 - 3 a : abcde
// 1 - 3 b : cdefg
// 2 - 9 c : ccccccccc
//
// Each line gives the password policy and then the password.The password policy indicates the lowest and highest number
// of times a given letter must appear for the password to be valid.For example, 1 - 3 a means that the password must
// contain a at least 1 time and at most 3 times.
//
// In the above example, 2 passwords are valid.The middle password, cdefg, is not; it contains no instances of b, but
// needs at least 1. The first and third passwords are valid : they contain one a or nine c, both within the limits of
// their respective policies.
//
// How many passwords are valid according to their policies ?
//
//-- - Part Two-- -
// While it appears you validated the passwords correctly, they don't seem to be what the Official Toboggan Corporate
// Authentication System is expecting.
//
// The shopkeeper suddenly realizes that he just accidentally explained the password policy rules from his old job at
// the sled rental place down the street!The Official Toboggan Corporate Policy actually works a little differently.
//
// Each policy actually describes two positions in the password, where 1 means the first character, 2 means the second
// character, and so on. (Be careful; Toboggan Corporate Policies have no concept of "index zero"!) Exactly one of these
// positions must contain the given letter.Other occurrences of the letter are irrelevant for the purposes of policy
// enforcement.
//
// Given the same example list from above :
//
// 1 - 3 a : abcde is valid : position 1 contains a and position 3 does not.
// 1 - 3 b : cdefg is invalid : neither position 1 nor position 3 contains b.
// 2 - 9 c : ccccccccc is invalid : both position 2 and position 9 contain c.
//
// How many passwords are valid according to the new interpretation of the policies ?

#include <algorithm>
#include <cassert>

#include <resources.hpp>

#include "utils.hpp"

//==============================================================================
struct Password_Policy {
    char character;
    int param_1; // for day_2_a it's min, for day_2_b it's index_1
    int param_2; // for day_2_a it's max, for day_2_b it's index_2
};

//==============================================================================
struct Entry {
    Password_Policy password_policy;
    std::string_view password;
};

//==============================================================================
Entry parse_entry(std::string_view const & line)
{
    static constexpr auto MIN_MAX_SEPARATOR = '-';
    static constexpr auto MAX_CHAR_SEPARATOR = ' ';

    auto const min_begin{ line.cbegin() };
    auto const min_end{ std::find(min_begin, line.cend(), MIN_MAX_SEPARATOR) };
    auto const max_begin{ min_end + 1 };
    auto const max_end{ std::find(max_begin, line.cend(), MAX_CHAR_SEPARATOR) };
    auto const character{ max_end + 1 };
    auto const password_begin{ character + 3 };

    int min;
    [[maybe_unused]] auto from_char_result{ std::from_chars(&*min_begin, &*min_end, min) };
    assert(from_char_result.ec == std::errc());

    int max;
    from_char_result = std::from_chars(&*max_begin, &*max_end, max);
    assert(from_char_result.ec == std::errc());

    Password_Policy const password_policy{ *character, min, max };
    auto const password_size{ static_cast<size_t>(line.cend() - password_begin) };
    std::string_view const password{ &*password_begin, password_size };
    Entry const entry{ password_policy, password };

    return entry;
}

//==============================================================================
bool is_valid_day_2_a_entry(Entry const & entry)
{
    auto const count{ std::count(entry.password.begin(), entry.password.end(), entry.password_policy.character) };
    auto const & min{ entry.password_policy.param_1 };
    auto const & max{ entry.password_policy.param_2 };

    return count >= min && count <= max;
}

//==============================================================================
bool is_valid_day_2_b_entry(Entry const & entry)
{
    auto const & character{ entry.password_policy.character };
    auto const & index_1{ entry.password_policy.param_1 };
    auto const & index_2{ entry.password_policy.param_2 };
    assert(index_1 > 0 && static_cast<size_t>(index_1) <= entry.password.size());
    assert(index_2 > 0 && static_cast<size_t>(index_2) <= entry.password.size());

    auto const index_1_matches{ entry.password[static_cast<size_t>(index_1) - 1] == character };
    auto const index_2_matches{ entry.password[static_cast<size_t>(index_2) - 1] == character };
    return index_1_matches != index_2_matches;
}

//==============================================================================
std::string day_2_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    std::vector<Entry> entries{ lines.size() };
    std::transform(lines.begin(), lines.end(), entries.begin(), parse_entry);
    auto const count{ std::count_if(entries.begin(), entries.end(), is_valid_day_2_a_entry) };

    return std::to_string(count);
}

//==============================================================================
std::string day_2_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    std::vector<Entry> entries{ lines.size() };
    std::transform(lines.begin(), lines.end(), entries.begin(), parse_entry);

    auto const count{ std::count_if(entries.begin(), entries.end(), is_valid_day_2_b_entry) };

    return std::to_string(count);
}