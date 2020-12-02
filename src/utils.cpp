#include "utils.hpp"

#include <algorithm>
#include <exception>
#include <fstream>

std::string read_file(char const * path)
{
    std::ifstream file{ path };

    if (!file.is_open()) {
        auto const error{ std::string{ "Can't open file \"" }.append(path).append("\".") };
        throw std::exception{ error.c_str() };
    }

    file.seekg(std::ios::end);
    auto const size{ file.tellg() };
    file.seekg(std::ios::beg);
    std::string result{};
    result.reserve(size);
    result.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return result;
}

std::vector<std::string_view> split(std::string const & string, char const separator)
{
    // locate separators
    std::vector<char const *> separators_ptr{};
    std::for_each(string.c_str(), string.c_str() + string.size(), [&separators_ptr, separator](char const & character) {
        if (character == separator) {
            separators_ptr.push_back(&character);
        }
    });

    std::vector<std::string_view> result{};
    result.reserve(separators_ptr.size() + 1);

    auto const * begin{ string.c_str() };
    for (auto const * end : separators_ptr) {
        result.emplace_back(begin, end - begin);
        begin = end + 1;
    }
    auto const size{ std::strlen(begin) };
    result.emplace_back(begin, size);

    return result;
}
