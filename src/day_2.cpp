#include <algorithm>
#include <cassert>

#include <resources/resources.hpp>

#include "utils.hpp"

struct Password_Policy {
    char character;
    int param_1; // for day_2_a it's min, for day_2_b it's index_1
    int param_2; // for day_2_a it's max, for day_2_b it's index_2
};

struct Entry {
    Password_Policy password_policy;
    std::string_view password;
};

std::vector<Entry> parse_entries(std::vector<std::string_view> const & lines)
{
    static constexpr auto MIN_MAX_SEPARATOR = '-';
    static constexpr auto MAX_CHAR_SEPARATOR = ' ';

    std::vector<Entry> result{};
    result.reserve(lines.size());

    for (auto const & line : lines) {
        auto const min_begin{ line.cbegin() };
        auto const min_end{ std::find(min_begin, line.cend(), MIN_MAX_SEPARATOR) };
        auto const max_begin{ min_end + 1 };
        auto const max_end{ std::find(max_begin, line.cend(), MAX_CHAR_SEPARATOR) };
        auto const character{ max_end + 1 };
        auto const password_begin{ character + 3 };

        int min;
        std::from_chars(&*min_begin, &*min_end, min);
        int max;
        std::from_chars(&*max_begin, &*max_end, max);
        Password_Policy const password_policy{ *character, min, max };
        auto const password_size{ static_cast<size_t>(line.cend() - password_begin) };
        Entry const entry{ password_policy, std::string_view{ &*password_begin, password_size } };
        result.push_back(entry);
    }

    return result;
}

bool is_valid_day_2_a_entry(Entry const & entry)
{
    auto const count{ std::count(entry.password.begin(), entry.password.end(), entry.password_policy.character) };
    auto const & min{ entry.password_policy.param_1 };
    auto const & max{ entry.password_policy.param_2 };
    return count >= min && count <= max;
}

bool is_valid_day_2_b_entry(Entry const & entry)
{
    auto const & index_1{ entry.password_policy.param_1 };
    auto const & index_2{ entry.password_policy.param_2 };
    assert(index_1 > 0 && static_cast<size_t>(index_1) <= entry.password.size());
    assert(index_2 > 0 && static_cast<size_t>(index_2) <= entry.password.size());
    auto const & character{ entry.password_policy.character };
    auto const index_1_matches{ entry.password[static_cast<size_t>(index_1) - 1] == character };
    auto const index_2_matches{ entry.password[static_cast<size_t>(index_2) - 1] == character };
    return index_1_matches != index_2_matches;
}

std::string day_2_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    auto const entries{ parse_entries(lines) };
    auto const count{ std::count_if(entries.begin(), entries.end(), [](Entry const & entry) -> bool {
        return is_valid_day_2_a_entry(entry);
    }) };

    return std::to_string(count);
}

std::string day_2_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    auto const entries{ parse_entries(lines) };
    auto const count{ std::count_if(entries.begin(), entries.end(), [](Entry const & entry) -> bool {
        return is_valid_day_2_b_entry(entry);
    }) };

    return std::to_string(count);
}