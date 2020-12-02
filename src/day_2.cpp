#include <algorithm>

#include <resources/resources.hpp>

#include "utils.hpp"

struct Password_Policy {
    char character;
    int min;
    int max;
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

bool is_valid_entry(Entry const & entry)
{
    auto const count{ std::count(entry.password.begin(), entry.password.end(), entry.password_policy.character) };
    return count >= entry.password_policy.min && count <= entry.password_policy.max;
}

std::string day_2_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    auto const entries{ parse_entries(lines) };
    auto const count{ std::count_if(entries.begin(), entries.end(), [](Entry const & entry) -> bool {
        return is_valid_entry(entry);
    }) };

    return std::to_string(count);
}

std::string day_2_b(char const * input_file_path)
{
    return "coucou";
}