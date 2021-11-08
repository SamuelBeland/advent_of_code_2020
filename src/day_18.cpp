#include "utils.hpp"
#include <resources.hpp>

namespace
{
using number_t = uint64_t;

[[nodiscard]] constexpr bool is_numeric(char const c) noexcept
{
    return c >= '0' && c <= '9';
}

//==============================================================================
number_t apply_op(number_t const & lhs, char const & op, number_t const & rhs)
{
    if (op == '+') {
        return lhs + rhs;
    }
    assert(op == '*');
    return lhs * rhs;
}

struct Term {
    number_t value;
    char const * end;
};

[[nodiscard]] char const * get_subexpression_end(char const * begin) noexcept
{
    assert(*begin == '(');
    unsigned depth{ 1 };
    do {
        ++begin;
        if (*begin == ')') {
            --depth;
        } else if (*begin == '(') {
            ++depth;
        }
    } while (depth > 0);
    return begin;
}

[[nodiscard]] number_t evaluate_expression(aoc::StringView const & expression);

[[nodiscard]] Term evaluate_first_expression_term(aoc::StringView const & expression)
{
    Term result;

    assert(!expression.empty());
    if (expression.startsWith('(')) {
        // is a sub-expression
        result.end = get_subexpression_end(expression.cbegin());
        result.value = evaluate_expression(aoc::StringView{ std::next(expression.cbegin()), result.end });
        ++result.end;
        return result;
    }

    // is just a numeric value!
    result.end = std::find_if_not(expression.cbegin(), expression.cend(), is_numeric);
    result.value = aoc::StringView{ expression.cbegin(), result.end }.parse<number_t>();

    return result;
}

[[nodiscard]] number_t evaluate_expression(aoc::StringView const & expression)
{
    // eval leftmost term
    auto leftmost_term{ evaluate_first_expression_term(expression) };

    auto & cur{ leftmost_term.end };
    auto & value{ leftmost_term.value };

    // eval rhs
    do {
        assert(*cur == ' ');
        auto const & op{ *(cur + 1) };
        assert(*(cur + 2) == ' ');
        cur += 3;
        auto const term{ evaluate_first_expression_term(aoc::StringView{ cur, expression.cend() }) };
        value = apply_op(value, op, term.value);
        cur = term.end;
    } while (cur != expression.cend());

    return value;
}

} // namespace

//==============================================================================
std::string day_18_a(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    auto const values{ aoc::StringView{ input }.iterate_transform(evaluate_expression, '\n') };
    auto const result{ aoc::reduce(values, number_t{}, std::plus()) };
    return std::to_string(result);
}

//==============================================================================
std::string day_18_b(char const * input_file_path)
{
    return "coucou";
}