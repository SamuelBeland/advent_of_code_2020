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

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

enum class Operator { plus, multiplies };

[[nodiscard]] constexpr Operator charToOperator(char const c)
{
    switch (c) {
    case '+':
        return Operator::plus;
    case '*':
        return Operator::multiplies;
    default:
        break;
    }
    assert(false);
    return {};
}

template<Operator Op1, Operator... Ops>
struct Precedence {
    static constexpr Operator op = Op1;
    using next = Precedence<Ops...>;
    [[nodiscard]] static constexpr bool has_precedence(Operator const a, Operator const b)
    {
        if (a == op) {
            return true;
        }
        if (b == op) {
            return false;
        }
        return next::has_precedence(a, b);
    }
};

template<Operator Op>
struct Precedence<Op> {
    static constexpr Operator op = Op;
};

template<typename T>
struct is_precedence {
    static constexpr bool value = false;
};

template<Operator... Ops>
struct is_precedence<Precedence<Ops...>> {
    static constexpr bool value = true;
};

template<typename T>
constexpr auto is_precedence_v = is_precedence<T>::value;

template<typename PrecedenceType>
[[nodiscard]] Term evaluate_first_term_in_expression(aoc::StringView const & string) noexcept
{
    static_assert(is_precedence_v<PrecedenceType>);

    Term result;

    if (string.startsWith('(')) {
        // parenthesis can be evaluated right away
        result.end = get_subexpression_end(string.cbegin()) + 1;
        result.value
            = evaluate_first_term_in_expression<PrecedenceType>(aoc::StringView{ string.begin() + 1, result.end - 1 });
        return result;
    }

    // a simple numeric value
    result.end = string.removeFromStart(1).find_if_not(is_numeric);
    result.value = aoc::StringView{ string.cbegin(), result.end }.parse<number_t>();
    return result;
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