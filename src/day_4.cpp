#include <algorithm>
#include <array>

#include "utils.hpp"
#include <resources.hpp>

template<typename T>
T parse_string(std::string_view const & string)
{
    T value;
    [[maybe_unused]] auto const from_char_result{
        std::from_chars(string.data(), string.data() + string.size(), value)
    };
    assert(from_char_result.ec == std::errc());
    return value;
}

//==============================================================================
bool is_byr_valid(std::string_view const & string)
{
    auto const value{ parse_string<unsigned>(string) };
    return value >= 1920 && value <= 2002;
}

//==============================================================================
bool is_iyr_valid(std::string_view const & string)
{
    auto const value{ parse_string<unsigned>(string) };
    return value >= 2010 && value <= 2020;
}

//==============================================================================
bool is_eyr_valid(std::string_view const & string)
{
    auto const value{ parse_string<unsigned>(string) };
    return value >= 2020 && value <= 2030;
}

//==============================================================================
enum class Height_Unit { cm, in };

//==============================================================================
bool is_hgt_valid(std::string_view const & string)
{
    auto const cm_begin{ string.find("cm") };
    if (cm_begin != std::string_view::npos) {
        auto const field{ string.substr(0, cm_begin) };
        auto const value{ parse_string<unsigned>(field) };
        return value >= 150 && value <= 193;
    }
    auto const in_begin{ string.find("in") };
    if (in_begin == std::string_view::npos) {
        return false;
    }

    auto const field{ string.substr(0, in_begin) };
    auto const value{ parse_string<unsigned>(field) };
    return value >= 59 && value <= 76;
}

//==============================================================================
bool is_hcl_valid(std::string_view const & string)
{
    static constexpr unsigned NUM_COLOR_CHARACTERS{ 6 };

    if (string.size() != 7) {
        return false;
    }

    auto const * ptr{ string.data() };
    if (*ptr++ != '#') {
        return false;
    }
    return std::all_of(ptr, ptr + NUM_COLOR_CHARACTERS, [](char const character) {
        return (character >= 'a' && character <= 'f') || (character >= '0' && character <= '9');
    });
}

//==============================================================================
bool is_ecl_valid(std::string_view const & string)
{
    return string == "amb" || string == "blu" || string == "brn" || string == "gry" || string == "grn"
           || string == "hzl" || string == "oth";
}

//==============================================================================
bool is_pid_valid(std::string_view const & string)
{
    static constexpr unsigned NUM_DIGITS{ 9 };

    if (string.size() != NUM_DIGITS) {
        return false;
    }
    auto const * ptr{ string.data() };
    return std::all_of(ptr, ptr + string.size(), [](char const character) {
        return character >= '0' && character <= '9';
    });
}

//==============================================================================
struct Constraint {
    std::string_view id;
    bool (*validate)(std::string_view const &);
};

//==============================================================================
static constexpr std::array<Constraint, 7> CONSTRAINTS{
    Constraint{ "byr:", is_byr_valid }, Constraint{ "iyr:", is_iyr_valid }, Constraint{ "eyr:", is_eyr_valid },
    Constraint{ "hgt:", is_hgt_valid }, Constraint{ "hcl:", is_hcl_valid }, Constraint{ "ecl:", is_ecl_valid },
    Constraint{ "pid:", is_pid_valid }
};

//==============================================================================
bool has_all_mandatory_fields(std::string_view const & entry)
{
    for (auto const & field : CONSTRAINTS) {
        if (entry.find(field.id) == std::string::npos) {
            return false;
        }
    }
    return true;
}

//==============================================================================
bool satisfies_constraint(std::string_view const & entry, Constraint const & constraint)
{
    static constexpr std::array<char, 2> VALUE_STOP_CHARS{ ' ', '\n' };

    auto const find_result{ entry.find(constraint.id) };
    if (find_result == std::string_view::npos) {
        return false;
    }
    auto const * const value_begin{ entry.data() + find_result + constraint.id.size() };
    auto const * const value_end{
        std::find_first_of(value_begin, entry.data() + entry.size(), VALUE_STOP_CHARS.cbegin(), VALUE_STOP_CHARS.cend())
    };
    std::string_view const value_string{ value_begin, static_cast<size_t>(value_end - value_begin) };
    auto const is_valid{ constraint.validate(value_string) };
    return is_valid;
}

//==============================================================================
bool is_valid_entry(std::string_view const & entry)
{
    auto const test_constraint
        = [&entry](Constraint const & constraint) { return satisfies_constraint(entry, constraint); };

    return std::all_of(CONSTRAINTS.cbegin(), CONSTRAINTS.cend(), test_constraint);
}

//==============================================================================
std::string day_4_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const entries{ split(input, "\n\n") };
    auto const count{ std::count_if(entries.cbegin(), entries.cend(), has_all_mandatory_fields) };
    return std::to_string(count);
}

//==============================================================================
std::string day_4_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const entries{ split(input, "\n\n") };
    auto const count{ std::count_if(entries.cbegin(), entries.cend(), is_valid_entry) };

    return std::to_string(count);
}