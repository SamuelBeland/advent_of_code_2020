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

#include "StringView.hpp"
#include "utils.hpp"

#include <resources.hpp>

namespace
{
//==============================================================================
struct Password_Policy {
    char character;
    int param_1; // for day_2_a it's min, for day_2_b it's index_1
    int param_2; // for day_2_a it's max, for day_2_b it's index_2
};

//==============================================================================
struct Entry {
    Password_Policy password_policy;
    aoc::StringView password;
    //==============================================================================
    static Entry from_string(aoc::StringView const & string)
    {
        Entry entry;
        string.scan("{}-{} {}: {}",
                    entry.password_policy.param_1,
                    entry.password_policy.param_2,
                    entry.password_policy.character,
                    entry.password);

        return entry;
    }
};

//==============================================================================
template<typename Pred>
std::string day_2(char const * input_file_path, Pred const & predicate)
{
    auto const input{ aoc::read_file(input_file_path) };
    auto const entries{ aoc::StringView{ input }.iterate_transform(Entry::from_string, '\n') };
    auto const count{ aoc::count_if(entries, predicate) };

    return std::to_string(count);
}

} // namespace

//==============================================================================
std::string day_2_a(char const * input_file_path)
{
    return day_2(input_file_path, [](Entry const & entry) {
        auto const char_count{ aoc::count(entry.password, entry.password_policy.character) };
        auto const & min{ entry.password_policy.param_1 };
        auto const & max{ entry.password_policy.param_2 };

        return char_count >= min && char_count <= max;
    });
}

//==============================================================================
std::string day_2_b(char const * input_file_path)
{
    return day_2(input_file_path, [](Entry const & entry) {
        auto const & character{ entry.password_policy.character };
        auto const & index_1{ entry.password_policy.param_1 };
        auto const & index_2{ entry.password_policy.param_2 };
        assert(index_1 > 0 && aoc::narrow<size_t>(index_1) <= entry.password.size());
        assert(index_2 > 0 && aoc::narrow<size_t>(index_2) <= entry.password.size());

        auto const index_1_matches{ entry.password[aoc::narrow<size_t>(index_1) - 1] == character };
        auto const index_2_matches{ entry.password[aoc::narrow<size_t>(index_2) - 1] == character };
        return index_1_matches != index_2_matches;
    });
}
