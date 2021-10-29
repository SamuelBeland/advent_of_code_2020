#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <numeric>
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
void copy_or_parse(std::string_view const & string, T & out_param)
{
    if constexpr (std::is_same_v<std::string_view, T> || std::is_same_v<std::string, T>) {
        out_param = string;
    } else if constexpr (std::is_same_v<char, T>) {
        assert(string.size() == 1);
        out_param = string.front();
    } else {
        [[maybe_unused]] auto const error{ std::from_chars(string.data(), string.data() + string.size(), out_param) };
        assert(error.ec == std::errc());
    }
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
            element_begin = std::next(element_end);
        };
        for (auto const & c : string) {
            if (c == separator) {
                call_func(&c);
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

    copy_or_parse(value, out_param);

    std::string_view const new_string{ value_end + suffix_length,
                                       string.size() - prefix_length - value_length - suffix_length };
    std::string_view const new_format{ format_suffix_end, format.size() - prefix_length - PARAM_SIZE - suffix_length };
    scan(new_string, new_format, Args...);
}

//==============================================================================
template<typename T>
void scan_number_list(std::string_view const & string, T & collection, std::string_view const & separator)
{
    auto const add_element = [&](std::string_view const & element) {
        typename T::value_type item;
        copy_or_parse(element, item);
        collection.push_back(item);
    };

    for_each_element_in_string(string, add_element, separator);
}

//==============================================================================
template<typename T>
void scan_number_list(std::string_view const & string, T & collection, char const separator)
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

template<typename Coll, typename Fn>
void transform(Coll & coll, Fn const & fn)
{
    std::transform(coll.begin(), coll.end(), fn);
}

template<typename In, typename Dest, typename Fn>
void transform(In const & coll, Dest & dest, Fn const & fn)
{
    std::transform(coll.cbegin(), coll.cend(), dest.begin(), fn);
}

template<typename In1, typename In2, typename Dest, typename Fn>
void transform(In1 const & in1, In2 const & in2, Dest & dest, Fn const & fn)
{
    std::transform(in1.cbegin(), in1.cend(), in2.cbegin(), dest.begin(), fn);
}

template<typename Coll, typename T, typename Fn>
[[nodiscard]] auto reduce(Coll const & coll, T && init, Fn const & fn)
{
    return std::reduce(coll.cbegin(), coll.cend(), std::forward<T>(init), fn);
}

template<typename Coll, typename T, typename Transform_Fn, typename Reduce_Fn>
[[nodiscard]] auto
    transform_reduce(Coll const & coll, T && init, Transform_Fn const & transform_fn, Reduce_Fn const & reduce_fn)
{
    return std::transform_reduce(coll.cbegin(), coll.cend(), std::forward<T>(init), reduce_fn, transform_fn);
}

template<typename In1, typename In2, typename T, typename Transform_Fn, typename Reduce_Fn>
[[nodiscard]] auto transform_reduce(In1 const & in1,
                                    In2 const & in2,
                                    T && init,
                                    Transform_Fn const & transform_fn,
                                    Reduce_Fn const & reduce_fn)
{
    return std::transform_reduce(in1.cbegin(),
                                 in1.cend(),
                                 in2.cbegin(),
                                 std::forward<T>(init),
                                 reduce_fn,
                                 transform_fn);
}

template<typename Coll>
void sort(Coll & coll)
{
    std::sort(coll.begin(), coll.end());
}

template<typename Coll>
Coll sort(Coll && coll)
{
    auto moved{ std::move(coll) };
    std::sort(moved.begin(), moved.end());
    return moved;
}

template<typename Coll, typename T>
[[nodiscard]] auto count(Coll const & coll, T const & value)
{
    return std::count(coll.cbegin(), coll.cend(), value);
}

template<typename Coll, typename Pred>
[[nodiscard]] auto count_if(Coll const & coll, Pred const & pred)
{
    return std::count_if(coll.cbegin(), coll.cend(), pred);
}

template<typename Coll, typename Pred>
[[nodiscard]] bool all_of(Coll const & coll, Pred const & pred)
{
    return std::all_of(coll.cbegin(), coll.cend(), pred);
}

template<typename Coll, typename Fn>
void for_each(Coll const & coll, Fn const & fn)
{
    std::for_each(coll.cbegin(), coll.cend(), fn);
}

template<typename Coll, typename Fn>
void for_each(Coll & coll, Fn const & fn)
{
    std::for_each(coll.begin(), coll.end(), fn);
}

template<typename Coll>
[[nodiscard]] auto max_element(Coll const & coll)
{
    return std::max_element(coll.cbegin(), coll.cend());
}

template<typename Coll, typename Pred>
[[nodiscard]] auto find_if(Coll & coll, Pred const & pred)
{
    return std::find_if(coll.begin(), coll.end(), pred);
}

template<typename Coll>
[[nodiscard]] auto unique(Coll & coll)
{
    return std::unique(coll.begin(), coll.end());
}

template<typename Coll, typename Pred>
auto remove_if(Coll & coll, Pred const & pred)
{
    auto const new_end{ std::remove_if(coll.begin(), coll.end(), pred) };
    auto const num_removed{ std::distance(new_end, coll.end()) };
    coll.erase(new_end, coll.end());
    return num_removed;
}

template<typename Coll, typename T>
void iota(Coll & coll, T const & init)
{
    std::iota(coll.begin(), coll.end(), init);
}

template<typename Coll, typename Pred>
auto partition(Coll & coll, Pred const & pred)
{
    return std::partition(coll.begin(), coll.end(), pred);
}

template<typename Coll, typename Pred>
[[nodiscard]] auto none_of(Coll const & coll, Pred const & pred)
{
    return std::none_of(coll.cbegin(), coll.cend(), pred);
}

template<typename Coll, typename Pred>
[[nodiscard]] auto any_of(Coll const & coll, Pred const & pred)
{
    return std::any_of(coll.cbegin(), coll.cend(), pred);
}

template<typename Coll, typename T>
[[nodiscard]] auto lower_bound(Coll const & coll, T const & value)
{
    assert(std::is_sorted(coll.cbegin(), coll.cend()));
    return std::lower_bound(coll.cbegin(), coll.cend(), value);
}

template<typename Coll, typename T>
[[nodiscard]] auto upper_bound(Coll const & coll, T const & value)
{
    assert(std::is_sorted(coll.cbegin(), coll.cend()));
    return std::upper_bound(coll.cbegin(), coll.cend(), value);
}