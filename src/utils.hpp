#pragma once

#include <cassert>
#include <charconv>
#include <string>
#include <string_view>
#include <vector>

//==============================================================================
std::string read_file(char const * path);

//==============================================================================
std::vector<std::string_view> split(std::string_view const & string, char separator = '\n');
std::vector<std::string_view> split(std::string_view const & string, std::string_view const & separator);

//==============================================================================
template<typename T>
std::vector<T> parse_numbers(std::vector<std::string_view> const & list)
{
    std::vector<T> result{};
    result.reserve(list.size());

    for (auto const & word : list) {
        T value;
        [[maybe_unused]] auto const error{ std::from_chars(word.data(), word.data() + word.size(), value) };
        assert(error.ec == std::errc());
        result.push_back(value);
    }

    return result;
}
