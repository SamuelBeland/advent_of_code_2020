#pragma once

#include <charconv>
#include <string>
#include <string_view>
#include <vector>

std::string read_file(char const * path);

std::vector<std::string_view> split(std::string const & string, char separator = '\n');

template<typename T>
std::vector<T> convert_list(std::vector<std::string_view> const & list)
{
    std::vector<T> result{};
    result.reserve(list.size());

    for (auto const & word : list) {
        T value;
        std::from_chars(word.data(), word.data() + word.size(), value);
        result.push_back(value);
    }

    return result;
}