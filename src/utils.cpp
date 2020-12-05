#include "utils.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>

//==============================================================================
std::string read_file(char const * path)
{
    std::ifstream file{ path };

    assert(file.is_open());

    file.seekg(std::ios::end);
    auto const size{ file.tellg() };
    file.seekg(std::ios::beg);

    std::string result{};
    result.reserve(size);

    result.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return result;
}

//==============================================================================
std::vector<std::string_view> split(std::string const & string, char const separator)
{
    // locate separators
    std::vector<char const *> separators_ptr{};
    std::for_each(string.c_str(), string.c_str() + string.size(), [&separators_ptr, separator](char const & character) {
        if (character == separator) {
            separators_ptr.push_back(&character);
        }
    });

    // insert separated string_views
    std::vector<std::string_view> result{};
    result.reserve(separators_ptr.size() + 1);

    auto const * begin{ string.c_str() };
    for (auto const * end : separators_ptr) {
        result.emplace_back(begin, end - begin);
        begin = end + 1;
    }

    // insert last element
    auto const length_of_last_element{ std::strlen(begin) };
    if (length_of_last_element > 0) {
        result.emplace_back(begin, length_of_last_element);
    }

    return result;
}

//==============================================================================
std::vector<std::string_view> split(std::string const & string, std::string const & separator)
{
    // locate separators
    std::vector<char const *> separators_ptr{};
    size_t current_index{ string.find(separator) };
    while (current_index != std::string::npos) {
        separators_ptr.push_back(string.data() + current_index);
        current_index = string.find(separator, current_index + separator.size());
    }

    // insert separated string_views
    std::vector<std::string_view> result{};
    result.reserve(separators_ptr.size() + 1);

    auto const * begin{ string.c_str() };
    for (auto const * end : separators_ptr) {
        result.emplace_back(begin, end - begin);
        begin = end + separator.size();
    }

    // insert last element
    auto const length_of_last_element{ std::strlen(begin) };
    if (length_of_last_element > 0) {
        result.emplace_back(begin, length_of_last_element);
    }

    return result;
}
