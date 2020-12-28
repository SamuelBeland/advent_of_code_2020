#pragma once

#include <algorithm>
#include <array>
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
template<typename T, typename Coll>
std::vector<T> parse_number_list(Coll const & collection)
{
    std::vector<T> result{};
    result.reserve(collection.size());

    for (auto const & word : collection) {
        T value;
        [[maybe_unused]] auto const error{ std::from_chars(word.data(), word.data() + word.size(), value) };
        assert(error.ec == std::errc());
        result.push_back(value);
    }

    return result;
}

//==============================================================================
template<typename T, typename It>
std::vector<T> parse_number_list(It begin, It const end)
{
    std::vector<T> result{};
    result.reserve(static_cast<size_t>(end - begin));

    while (begin != end) {
        T value;
        [[maybe_unused]] auto const error{ std::from_chars(begin->data(), begin->data() + begin->size(), value) };
        assert(error.ec == std::errc());
        result.push_back(value);
        ++begin;
    }

    return result;
}

//==============================================================================
template<typename T, size_t MAX_SIZE>
class Static_Vector
{
    std::array<T, MAX_SIZE> m_data;
    size_t m_size{};

public:
    using value_type = T;

    T * begin() { return m_data.data(); }
    T * end() { return m_data.data() + m_size; }
    T const * begin() const { return m_data.data(); }
    T const * end() const { return m_data.data() + m_size; }
    T const * cbegin() const { return m_data.data(); }
    T const * cend() const { return m_data.data() + m_size; }

    T & push_back(T const & new_element)
    {
        assert(m_size < MAX_SIZE);
        return m_data[m_size++] = new_element;
    }

    size_t size() const { return m_size; }
};

//==============================================================================
template<typename T>
void copy_or_parse(std::string_view const & string, T & out_param)
{
    [[maybe_unused]] auto const error{ std::from_chars(string.data(), string.data() + string.size(), out_param) };
    assert(error.ec == std::errc());
}

//==============================================================================
template<>
inline void copy_or_parse(std::string_view const & string, std::string_view & out_param)
{
    out_param = string;
}

//==============================================================================
template<>
inline void copy_or_parse(std::string_view const & string, std::string & out_param)
{
    out_param = string;
}

//==============================================================================
template<>
inline void copy_or_parse(std::string_view const & string, char & out_param)
{
    assert(string.size() == 1);
    out_param = string.front();
}

//==============================================================================
template<typename... Ts>
void scan(std::string_view const & string, std::string_view const & format)
{
}

//==============================================================================
// TODO : This is a pretty useful function. Optimize the hell out of it!
template<typename T, typename... Ts>
void scan(std::string_view const & string, std::string_view const & format, T & out_param, Ts &... Args)
{
    static constexpr auto OPEN_PARAM = '{';
    [[maybe_unused]] static constexpr auto CLOSE_PARAM = '}';
    static constexpr size_t PARAM_SIZE = 2;

    auto const * const format_begin{ format.data() };
    auto const * const format_end{ format.data() + format.size() };

    auto const * const format_prefix_end{ std::find(format_begin, format_end, OPEN_PARAM) };
    auto const prefix_length{ static_cast<size_t>(format_prefix_end - format_begin) };
    auto const prefix{ format.substr(0, prefix_length) };

    auto const * const format_suffix_begin{ format_prefix_end + PARAM_SIZE };
    auto const * const format_suffix_end{ std::find(format_suffix_begin, format_end, OPEN_PARAM) };
    auto const suffix_length{ static_cast<size_t>(format_suffix_end - format_suffix_begin) };
    std::string_view const suffix{ format_suffix_begin, suffix_length };

    auto const * const string_begin{ string.data() };
    auto const * const string_end{ string.data() + string.size() };

    auto const * const value_begin{ std::search(string_begin, string_end, prefix.cbegin(), prefix.cend())
                                    + prefix_length };
    auto const * const value_end{ suffix_length ? std::search(value_begin, string_end, suffix.cbegin(), suffix.cend())
                                                : string_end };
    auto const value_length{ static_cast<size_t>(value_end - value_begin) };
    std::string_view const value{ value_begin, value_length };

    copy_or_parse(value, out_param);

    std::string_view const new_string{ value_end + suffix_length,
                                       string.size() - prefix_length - value_length - suffix_length };
    std::string_view const new_format{ format_suffix_end, format.size() - prefix_length - PARAM_SIZE - suffix_length };
    scan(new_string, new_format, Args...);
}

//==============================================================================
template<typename T>
void scan_list(std::string_view const & string, T & collection, std::string_view const & separator)
{
    size_t begin{};
    auto end{ string.find(separator) };
    while (end != std::string_view::npos) {
        typename T::value_type item;
        auto const value_string{ string.substr(begin, end - begin) };
        copy_or_parse(value_string, item);
        collection.push_back(item);
        begin = end + separator.size();
        end = string.find(separator, begin);
    }
    if (begin < string.size()) {
        typename T::value_type item;
        auto const value_string{ string.substr(begin) };
        copy_or_parse(value_string, item);
        collection.push_back(item);
    }
}

//==============================================================================
template<typename T>
void scan_list(std::string_view const & string, T & collection, char const separator)
{
    size_t begin{};
    auto end{ string.find(separator) };
    while (end != std::string_view::npos) {
        typename T::value_type item;
        auto const value_string{ string.substr(begin, end - begin) };
        copy_or_parse(value_string, item);
        collection.push_back(item);
        begin = end + 1;
        end = string.find(separator, begin);
    }
    if (begin < string.size()) {
        typename T::value_type item;
        auto const value_string{ string.substr(begin) };
        copy_or_parse(value_string, item);
        collection.push_back(item);
    }
}

//==============================================================================
template<typename T>
std::vector<T> scan_list(std::string_view const & string, std::string_view const & separator)
{
    std::vector<T> result{};
    scan_list(string, result, separator);
    return result;
}

//==============================================================================
template<typename T>
std::vector<T> scan_list(std::string_view const & string, char const separator)
{
    std::vector<T> result{};
    scan_list(string, result, separator);
    return result;
}
