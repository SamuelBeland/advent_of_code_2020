#pragma once

#include "shortcuts.hpp"

#include <array>
#include <charconv>
#include <string>
#include <string_view>
#include <vector>

//==============================================================================
std::string read_file(char const * path);

//==============================================================================
std::vector<std::string_view> split(std::string_view const & string, char separator = '\n');
std::vector<std::string_view> split(std::string_view const & string, std::string_view const & separator);

#if defined(NDEBUG)
static constexpr auto IS_DEBUG = false;
#else
static constexpr auto IS_DEBUG = true;
#endif

//==============================================================================
template<typename To, typename From>
To narrow(From const & from) noexcept(!IS_DEBUG)
{
    if constexpr (IS_DEBUG) {
        assert(std::is_signed_v<To> || from >= 0);
        To const newValue{ static_cast<To>(from) };
        assert(static_cast<From>(newValue) == from);
        return newValue;
    } else {
        return static_cast<To>(from);
    }
}

//==============================================================================
template<typename T>
T copy_or_parse(std::string_view const & string)
{
    T value;
    if constexpr (std::is_same_v<std::string_view, T> || std::is_same_v<std::string, T>) {
        value = string;
    } else if constexpr (std::is_same_v<char, T>) {
        assert(string.size() == 1);
        value = string.front();
    } else {
        [[maybe_unused]] auto const error{ std::from_chars(string.data(), string.data() + string.size(), value) };
        assert(error.ec == std::errc());
    }
    return value;
}

//==============================================================================
template<typename Func, typename Separator>
void for_each_element_in_string(std::string_view const & string, Func const & func, Separator const & separator)
{
    if constexpr (std::is_same_v<Separator, char>) {
        auto const * element_begin{ string.data() };
        auto const call_func = [&](char const * element_end) {
            auto const size{ narrow<std::size_t>(std::distance(element_begin, element_end)) };
            func(std::string_view{ element_begin, size });
        };
        for (auto const & c : string) {
            if (c == separator) {
                call_func(&c);
                element_begin = std::next(&c);
            }
        }
        call_func(string.data() + string.size());
    } else if constexpr (std::is_same_v<Separator, std::string_view>) {
        auto element_begin{ string.cbegin() };

        auto const find_next_separator
            = [&]() { return std::search(element_begin, string.cend(), separator.cbegin(), separator.cend()); };

        auto const call_func = [&](std::string_view::const_iterator const & separator_begin) {
            auto const size{ narrow<std::size_t>(std::distance(element_begin, separator_begin)) };
            func(std::string_view{ &*element_begin, size });
        };

        for (auto separator_begin{ find_next_separator() }; separator_begin != string.cend();
             separator_begin = find_next_separator()) {
            call_func(separator_begin);
            element_begin = separator_begin + separator.size();
        }
        call_func(string.cend());
    } else if constexpr (std::is_convertible_v<Separator, std::string_view>) {
        for_each_element_in_string(string, func, std::string_view{ separator });
    } else {
        static_assert(false);
    }
}

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
    result.reserve(narrow<size_t>(end - begin));

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

    static_assert(std::is_trivially_destructible<T>::value, "Static_Vector value type must be trivially destructible.");

public:
    //==============================================================================
    using value_type = T;

    [[nodiscard]] auto begin() { return m_data.begin(); }
    [[nodiscard]] auto end() { return m_data.begin() + m_size; }
    [[nodiscard]] auto begin() const { return m_data.begin(); }
    [[nodiscard]] auto end() const { return m_data.begin() + m_size; }
    [[nodiscard]] auto cbegin() const { return m_data.cbegin(); }
    [[nodiscard]] auto cend() const { return m_data.cbegin() + m_size; }

    T & push_back(T const & new_element)
    {
        assert(m_size < MAX_SIZE);
        return m_data[m_size++] = new_element;
    }

    [[nodiscard]] size_t size() const { return m_size; }
};

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
    auto const prefix_length{ narrow<size_t>(format_prefix_end - format_begin) };
    auto const prefix{ format.substr(0, prefix_length) };

    auto const * const format_suffix_begin{ format_prefix_end + PARAM_SIZE };
    auto const * const format_suffix_end{ std::find(format_suffix_begin, format_end, OPEN_PARAM) };
    auto const suffix_length{ narrow<size_t>(format_suffix_end - format_suffix_begin) };
    std::string_view const suffix{ format_suffix_begin, suffix_length };

    auto const * const string_begin{ string.data() };
    auto const * const string_end{ string.data() + string.size() };

    auto const * const value_begin{ std::search(string_begin, string_end, prefix.cbegin(), prefix.cend())
                                    + prefix_length };
    auto const * const value_end{ suffix_length ? std::search(value_begin, string_end, suffix.cbegin(), suffix.cend())
                                                : string_end };
    auto const value_length{ narrow<size_t>(value_end - value_begin) };
    std::string_view const value{ value_begin, value_length };

    out_param = copy_or_parse<T>(value);

    std::string_view const new_string{ value_end + suffix_length,
                                       string.size() - prefix_length - value_length - suffix_length };
    std::string_view const new_format{ format_suffix_end, format.size() - prefix_length - PARAM_SIZE - suffix_length };
    scan(new_string, new_format, Args...);
}

//==============================================================================
template<typename T, typename U>
void scan_number_list(std::string_view const & string, T & collection, U const & separator)
{
    auto const add_element = [&](std::string_view const & element) {
        collection.push_back(copy_or_parse<typename T::value_type>(element));
    };

    for_each_element_in_string(string, add_element, separator);
}

//==============================================================================
template<typename T>
[[nodiscard]] std::vector<T> scan_number_list(std::string_view const & string, std::string_view const & separator)
{
    std::vector<T> result{};
    scan_number_list(string, result, separator);
    return result;
}

//==============================================================================
template<typename T>
[[nodiscard]] std::vector<T> scan_number_list(std::string_view const & string, char const separator)
{
    std::vector<T> result{};
    scan_number_list(string, result, separator);
    return result;
}
