#include "utils.hpp"
#include <resources.hpp>

namespace
{
using number_t = unsigned;

//==============================================================================
number_t apply_op(number_t const & lhs, char const & op, number_t const & rhs)
{
    if (op == '+') {
        return lhs + rhs;
    }
    assert(op == '*');
    return lhs * rhs;
}

//==============================================================================
aoc::StringView isolate_subexpression(char const * begin) noexcept(!aoc::detail::IS_DEBUG)
{
    assert(*begin == '(');
    unsigned depth{ 1 };
    auto const * cur{ std::next(begin) };
    for (; depth > 0; ++cur) {
        if (*cur == '(') {
            ++depth;
            continue;
        }
        if (*cur == ')') {
            --depth;
        }
    }
    return aoc::StringView{ begin + 1, cur };
}

//==============================================================================
number_t evaluate(aoc::StringView const & string) noexcept(!aoc::detail::IS_DEBUG)
{
    assert(!string.empty());
    if (string.is_numeric()) {
        // just a number!
        return string.parse<number_t>();
    }

    auto const lhs_string{ string.front() == '(' ? isolate_subexpression(string.cbegin()) : string.upTo(' ') };
    auto value{ evaluate(lhs_string) };

    assert(*lhs_string.end() == ' ');
    auto const * cur{ lhs_string.end() };

    do {
        ++cur;
        auto const & op_char{ *cur };
        assert(*std::next(cur) == ' ');
        cur += 2;
        assert(cur < string.cend());
        auto const rhs_string{ *cur == '(' ? isolate_subexpression(cur)
                                           : aoc::StringView{ cur, string.cend() }.upTo(' ') };
        auto const rhs{ evaluate(rhs_string) };
        value = apply_op(value, op_char, rhs);
        cur = rhs_string.cend();
    } while (cur != string.cend());

    return value;
}

} // namespace

//==============================================================================
std::string day_18_a(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    auto const values{ aoc::StringView{ input }.iterate_transform(evaluate, '\n') };
    auto const result{ aoc::reduce(values, number_t{}, std::plus()) };
    return std::to_string(result);
}

//==============================================================================
std::string day_18_b(char const * input_file_path)
{
    return "coucou";
}