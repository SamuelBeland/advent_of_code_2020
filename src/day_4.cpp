#include <algorithm>
#include <array>

#include "utils.hpp"
#include <resources.hpp>

static const std::array<std::string, 8> MANDATORY_FIELDS{ "byr:", "iyr:", "eyr:", "hgt:", "hcl:", "ecl:", "pid:" };

//==============================================================================
bool has_all_mandatory_fields(std::string_view const & entry)
{
    for (auto const & field : MANDATORY_FIELDS) {
        if (entry.find(field) == std::string::npos) {
            return false;
        }
    }
    return true;
}

//==============================================================================
std::string day_4_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const text_entries{ split(input, "\n\n") };
    auto const count{ std::count_if(text_entries.cbegin(), text_entries.cend(), has_all_mandatory_fields) };
    return std::to_string(count);
}

//==============================================================================
std::string day_4_b(char const * input_file_path)
{
    return "coucou";
}