#include "utils.hpp"
#include <resources.hpp>

#include <variant>

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
    if (expression.starts_with('(')) {
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

struct Expression {
    using sub_expressions_t = std::variant<number_t, Expression *>;

    sub_expressions_t lhs;
    sub_expressions_t rhs;
    char op;

    ~Expression()
    {
        for (auto const & sub_expression : { lhs, rhs }) {
            if (std::holds_alternative<Expression *>(sub_expression)) {
                delete std::get<Expression *>(sub_expression);
            }
        }
    }

    [[nodiscard]] number_t evaluate() const noexcept(!aoc::detail::IS_DEBUG)
    {
        static auto const get_value = [](sub_expressions_t const & var) -> number_t {
            if (std::holds_alternative<number_t>(var)) {
                return std::get<number_t>(var);
            }
            return std::get<Expression *>(var)->evaluate();
        };
        auto const lhs_value{ get_value(lhs) };
        auto const rhs_value{ get_value(rhs) };
        return apply_op(lhs_value, op, rhs_value);
    }

    [[nodiscard]] static Expression parse(aoc::StringView const & string) noexcept(!aoc::detail::IS_DEBUG)
    {
        Expression result{};

        // do the magic

        auto const first_term{ string.until_false(is_numeric) };
        assert(first_term.cend() + 3 < string.cend());
        assert(*first_term.cend() == ' ');
        result.op = *std::next(first_term.cend());
        assert(*(first_term.cend() + 2) == ' ');
        auto const second_term{ aoc::StringView{ first_term.cend() + 3, string.cend() }.until_false(is_numeric) };
    }
};

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