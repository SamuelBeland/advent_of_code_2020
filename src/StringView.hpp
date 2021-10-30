#pragma once

#include "narrow.hpp"
#include "shortcuts.hpp"

#include <algorithm>
#include <charconv>
#include <functional>
#include <string>

namespace aoc
{
class StringView;

namespace detail
{
//==============================================================================
static constexpr auto FORMAT_CAPTURE_PREFIX = '{';

//==============================================================================
template<class InputIt1, class InputIt2>
[[nodiscard]] constexpr bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2);

//==============================================================================
template<typename...>
void scan_no_prefix(StringView const & /*string*/, StringView const & /*format*/) noexcept(!detail::IS_DEBUG)
{
}

//==============================================================================
template<typename T, typename... Ts>
void scan_no_prefix(StringView const & string,
                    StringView const & format,
                    T & out_param,
                    Ts &... other_out_params) noexcept(!detail::IS_DEBUG);

//==============================================================================
[[nodiscard]] constexpr std::size_t strlen(char const * str) noexcept
{
    size_t result{};
    while (str[result] != '\0') {
        ++result;
    }
    return result;
}

} // namespace detail

//==============================================================================
class StringView
{
    char const * m_data;
    std::size_t m_size;

public:
    //==============================================================================
    StringView() = default;
    StringView(std::string const & string) noexcept : m_data(string.data()), m_size(string.size()) {}
    constexpr StringView(char const * str) noexcept : m_data(str), m_size(detail::strlen(str)) {}
    constexpr StringView(char const * begin, char const * end) noexcept(!detail::IS_DEBUG)
        : m_data(begin)
        , m_size(aoc::narrow<std::size_t>(std::distance(begin, end)))
    {
        assert(begin <= end);
    }
    constexpr StringView(char const * begin, std::size_t const size) noexcept : m_data(begin), m_size(size) {}
    ~StringView() = default;
    //==============================================================================
    StringView(StringView const &) = default;
    StringView(StringView &&) = default;
    StringView & operator=(StringView const &) = default;
    StringView & operator=(StringView &&) = default;
    //==============================================================================
    [[nodiscard]] constexpr char const * begin() const { return m_data; }
    [[nodiscard]] constexpr char const * end() const { return m_data + m_size; }
    [[nodiscard]] constexpr char const * cbegin() const { return m_data; }
    [[nodiscard]] constexpr char const * cend() const { return m_data + m_size; }
    //==============================================================================
    [[nodiscard]] constexpr bool operator==(StringView const & other) const noexcept
    {
        if (m_size != other.m_size) {
            return false;
        }
        return detail::equal(cbegin(), cend(), other.cbegin());
    }

    [[nodiscard]] constexpr bool operator!=(StringView const & other) const noexcept { return !(*this == other); }
    //==============================================================================
    [[nodiscard]] constexpr std::size_t size() const noexcept { return m_size; }
    [[nodiscard]] constexpr bool empty() const noexcept { return m_size == 0; }
    [[nodiscard]] constexpr std::size_t count(char const c) const noexcept
    {
        std::size_t result{};
        for (auto const & c_ : *this) {
            if (c_ == c) {
                ++result;
            }
        }
        return result;
    }
    [[nodiscard]] std::size_t count(StringView const & other) const noexcept
    {
        std::size_t result{};
        auto const * element_end{ find(other) };
        while (element_end != cend()) {
            ++result;
            element_end = StringView{ element_end + other.size(), cend() }.find(other);
        }
        return result;
    }
    //==============================================================================
    [[nodiscard]] constexpr char const * find(char const c) const noexcept
    {
        char const * cur{ cbegin() };
        while (cur != cend() && *cur != c) {
            ++cur;
        }
        return cur;
    }
    [[nodiscard]] char const * find(StringView const & other) const noexcept
    {
        return std::search(cbegin(), cend(), std::boyer_moore_searcher(other.cbegin(), other.cend()));
    }
    //==============================================================================
    [[nodiscard]] constexpr char const & front() const noexcept(!detail::IS_DEBUG)
    {
        assert(m_size != 0);
        return *m_data;
    }
    //==============================================================================
    [[nodiscard]] constexpr StringView upTo(char const c) const noexcept { return StringView{ cbegin(), find(c) }; }
    [[nodiscard]] StringView upTo(StringView const & other) const noexcept
    {
        if (other.empty()) {
            return *this;
        }
        return StringView{ cbegin(), find(other) };
    }
    [[nodiscard]] constexpr StringView startingAfter(char const c) const noexcept
    {
        auto const * find_result{ find(c) };
        auto const * new_begin{ find_result == cend() ? find_result : std::next(find_result) };
        return StringView{ new_begin, cend() };
    }
    [[nodiscard]] StringView startingAfter(StringView const & other) const noexcept
    {
        // if (other.empty()) {
        //    return *this;
        //}
        auto const * find_result{ find(other) };
        auto const * new_begin{ find_result == cend() ? find_result : find_result + other.size() };
        return StringView{ new_begin, cend() };
    }
    //==============================================================================
    [[nodiscard]] constexpr StringView removeFromStart(std::size_t const sizeToRemove) const noexcept
    {
        auto const effectiveSizeToRemove{ std::min(m_size, sizeToRemove) };
        return StringView{ m_data + effectiveSizeToRemove, m_size - effectiveSizeToRemove };
    }
    //==============================================================================
    template<typename... Ts>
    void scan(StringView const & format, Ts &... out_params) const noexcept(!detail::IS_DEBUG)
    {
        auto const prefix_to_match{ format.upTo(detail::FORMAT_CAPTURE_PREFIX) };
        auto const string_without_prefix{ startingAfter(prefix_to_match) };
        StringView const format_without_prefix{ format.cbegin() + prefix_to_match.size(), format.cend() };

        detail::scan_no_prefix(string_without_prefix, format_without_prefix, out_params...);
    }
    //==============================================================================
    template<typename T>
    [[nodiscard]] T parse() const noexcept(!detail::IS_DEBUG)
    {
        T value;
        if constexpr (std::is_same_v<StringView, T> || std::is_same_v<std::string, T>) {
            value = *this;
        } else if constexpr (std::is_same_v<char, T>) {
            assert(m_size == 1);
            value = front();
        } else if constexpr (std::is_same_v<std::string_view, T>) {
            value = std::string_view{ cbegin(), m_size };
        } else {
            [[maybe_unused]] auto const error{ std::from_chars(cbegin(), cend(), value) };
            assert(error.ec == std::errc());
        }
        return value;
    }
    //==============================================================================
    template<typename T, typename Separator>
    [[nodiscard]] std::vector<T> parse_list(Separator const & separator) const noexcept(!detail::IS_DEBUG)
    {
        auto const size{ count(separator) + 1 };
        std::vector<T> result{};
        result.resize(size);
        auto cur{ result.begin() };

        auto const parse_and_add = [&](StringView const & string) { *cur++ = string.parse<T>(); };

        iterate(parse_and_add, separator);

        return result;
    }
    //==============================================================================
    template<typename T, typename Separator>
    [[nodiscard]] std::vector<T> parse_list_and_sort(Separator const & separator) const noexcept(!detail::IS_DEBUG)
    {
        auto result{ parse_list<T>(separator) };
        aoc::sort(result);
        return result;
    }
    //==============================================================================
    template<typename Func>
    constexpr void iterate(Func const & func, char const separator) const noexcept
    {
        StringView cur{ cbegin(), find(separator) };

        while (cur.cend() != cend()) {
            func(cur);
            StringView const leftover{ std::next(cur.cend()), cend() };
            cur = leftover.upTo(separator);
        }
        func(cur);
    }

    template<typename Func>
    void iterate(Func const & func, StringView const & separator) const noexcept
    {
        StringView cur{ cbegin(), find(separator) };

        while (cur.cend() != cend()) {
            func(cur);
            StringView const leftover{ cur.cend() + separator.size(), cend() };
            cur = leftover.upTo(separator);
        }
        func(cur);
    }

    template<typename Func, typename Separator>
    auto iterator_transform(Func const & func, Separator const & separator)
    {
        using value_type = decltype(func(StringView{}));
        std::vector<value_type> result{};
        result.resize(count(separator) + 1);
        auto cur{ result.begin() };

        auto const transform_and_add = [&](StringView const & string) { *cur++ = func(string); };

        iterate(transform_and_add, separator);

        return result;
    }
};

namespace detail
{
template<class InputIt1, class InputIt2>
[[nodiscard]] constexpr bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2)
{
    for (; first1 != last1; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return true;
}

template<typename T, typename... Ts>
void scan_no_prefix(StringView const & string,
                    StringView const & format,
                    T & out_param,
                    Ts &... other_out_params) noexcept(!detail::IS_DEBUG)
{
    assert(!string.empty());
    assert(format.front() == FORMAT_CAPTURE_PREFIX);
    assert(format.size() >= 2);

    auto const suffix_to_match{ StringView{ format.cbegin() + 2 }.upTo(FORMAT_CAPTURE_PREFIX) };
    auto const string_to_parse{ string.upTo(suffix_to_match) };

    out_param = string_to_parse.parse<T>();

    StringView const leftover_string{ string_to_parse.cend() + suffix_to_match.size(), string.cend() };
    StringView const leftover_format{ suffix_to_match.cend(), format.cend() };

    scan_no_prefix(leftover_string, leftover_format, other_out_params...);
}
} // namespace detail

} // namespace aoc