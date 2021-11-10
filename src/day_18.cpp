//--- Day 18: Operation Order ---
//
// As you look out the window and notice a heavily-forested continent slowly appear over the horizon, you are
// interrupted by the child sitting next to you. They're curious if you could help them with their math homework.
//
// Unfortunately, it seems like this "math" follows different rules than you remember.
//
// The homework (your puzzle input) consists of a series of expressions that consist of addition (+), multiplication
// (*), and parentheses ((...)). Just like normal math, parentheses indicate that the expression inside must be
// evaluated before it can be used by the surrounding expression. Addition still finds the sum of the numbers on both
// sides of the operator, and multiplication still finds the product.
//
// However, the rules of operator precedence have changed. Rather than evaluating multiplication before addition, the
// operators have the same precedence, and are evaluated left-to-right regardless of the order in which they appear.
//
// For example, the steps to evaluate the expression 1 + 2 * 3 + 4 * 5 + 6 are as follows:
//
// 1 + 2 * 3 + 4 * 5 + 6
//  3   * 3 + 4 * 5 + 6
//      9   + 4 * 5 + 6
//         13   * 5 + 6
//             65   + 6
//                 71
//
// Parentheses can override this order; for example, here is what happens if parentheses are added to form 1 + (2 * 3) +
// (4 * (5 + 6)):
//
// 1 + (2 * 3) + (4 * (5 + 6))
// 1 +    6    + (4 * (5 + 6))
//     7      + (4 * (5 + 6))
//     7      + (4 *   11   )
//     7      +     44
//            51
//
// Here are a few more examples:
//
// 2 * 3 + (4 * 5) becomes 26.
// 5 + (8 * 3 + 9 + 3 * 4 * 3) becomes 437.
// 5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4)) becomes 12240.
//((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2 becomes 13632.
//
// Before you can help with the homework, you need to understand it yourself. Evaluate the expression on each line of
// the homework; what is the sum of the resulting values?
//
//--- Part Two ---
//
// You manage to answer the child's questions and they finish part 1 of their homework, but get stuck when they reach
// the next section: advanced math.
//
// Now, addition and multiplication have different precedence levels, but they're not the ones you're familiar with.
// Instead, addition is evaluated before multiplication.
//
// For example, the steps to evaluate the expression 1 + 2 * 3 + 4 * 5 + 6 are now as follows:
//
// 1 + 2 * 3 + 4 * 5 + 6
//  3   * 3 + 4 * 5 + 6
//  3   *   7   * 5 + 6
//  3   *   7   *  11
//     21       *  11
//         231
//
// Here are the other examples from above:
//
// 1 + (2 * 3) + (4 * (5 + 6)) still becomes 51.
// 2 * 3 + (4 * 5) becomes 46.
// 5 + (8 * 3 + 9 + 3 * 4 * 3) becomes 1445.
// 5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4)) becomes 669060.
//((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2 becomes 23340.
//
// What do you get if you add up the results of evaluating the homework problems using these new rules?

#include "utils.hpp"
#include <resources.hpp>

namespace
{
using number_t = uint64_t;
using priority_t = unsigned;

constexpr priority_t PAREN_PRIORITY = 1000;

constexpr auto is_numeric = [](char const c) -> bool { return c >= '0' && c <= '9'; };

struct Operator;

struct Term {
    Operator * previous_op;
    Operator * next_op;
    number_t value;
};

struct Operator {
    Term * lhs;
    Term * rhs;
    unsigned priority;
    char c;

    [[nodiscard]] constexpr bool operator<(Operator const & other) const noexcept(!aoc::detail::IS_DEBUG)
    {
        return priority > other.priority;
    }
};

[[nodiscard]] constexpr bool sort_operator_ptrs(Operator const * lhs,
                                                Operator const * rhs) noexcept(!aoc::detail::IS_DEBUG)
{
    assert(lhs);
    assert(rhs);
    return *lhs < *rhs;
}

struct Expression {
    std::unique_ptr<Term> first_term;
    std::vector<Operator *> operators;

    number_t solve()
    {
        assert(first_term);
        assert(first_term->previous_op == nullptr);
        assert(std::is_sorted(operators.cbegin(), operators.cend(), sort_operator_ptrs));
        for (auto * op : operators) {
            assert(op);
            assert(op->lhs);
            assert(op->rhs);
            assert(op->c == '+' || op->c == '*');

            auto & lhs{ op->lhs };
            auto & rhs{ op->rhs };

            lhs->value = op->c == '+' ? lhs->value + rhs->value : lhs->value * rhs->value;

            if (rhs->next_op) {
                lhs->next_op = rhs->next_op;
                rhs->next_op->lhs = lhs;
            } else {
                lhs->next_op = nullptr;
            }

            delete op->rhs;
            delete op;
        }

        assert(first_term);
        assert(first_term->next_op == nullptr);
        return first_term->value;
    }

    template<typename OperatorPriorityFunc>
    static Expression parse(aoc::StringView const & string, OperatorPriorityFunc const & operator_priority_func)
    {
        assert(!string.empty());

        unsigned current_priority{};
        auto const * c{ string.cbegin() };

        while (*c == '(') {
            current_priority += PAREN_PRIORITY;
            ++c;
        }

        Expression result{};
        result.first_term = std::make_unique<Term>();
        Term * last_term{ result.first_term.get() };
        auto const * first_term_end{ std::find_if_not(std::next(c), string.cend(), is_numeric) };
        last_term->value = aoc::StringView{ c, first_term_end }.parse<number_t>();
        c = first_term_end;

        while (c != string.cend()) {
            // find next operator
            assert(*c == ' ');
            ++c;
            auto * last_operator{
                new Operator{ last_term, nullptr, current_priority + operator_priority_func(*c), *c }
            };
            last_term->next_op = last_operator;
            result.operators.push_back(last_operator);
            ++c;
            // find next term
            assert(*c == ' ');
            ++c;
            while (*c == '(') {
                current_priority += PAREN_PRIORITY;
                ++c;
            }
            assert(is_numeric(*c));
            auto const * term_end{ std::find_if_not(c, string.cend(), is_numeric) };
            last_term = new Term{ last_operator, nullptr, aoc::StringView{ c, term_end }.parse<number_t>() };
            last_operator->rhs = last_term;
            c = term_end;
            while (c != string.cend() && *c == ')') {
                assert(current_priority >= PAREN_PRIORITY);
                current_priority -= PAREN_PRIORITY;
                ++c;
            }
        }

        std::sort(result.operators.begin(), result.operators.end(), sort_operator_ptrs);
        return result;
    }
};

} // namespace

//==============================================================================
std::string day_18_a(char const * input_file_path)
{
    static auto const get_op_priority = [](char) { return 0; };
    static auto const parse_expression
        = [](aoc::StringView const & line) { return Expression::parse(line, get_op_priority); };

    auto const input{ aoc::read_file(input_file_path) };
    auto expressions{ aoc::StringView{ input }.iterate_transform(parse_expression, '\n') };
    auto const result{ std::transform_reduce(expressions.begin(),
                                             expressions.end(),
                                             number_t{},
                                             std::plus(),
                                             [](Expression & expression) { return expression.solve(); }) };
    return std::to_string(result);
}

//==============================================================================
std::string day_18_b(char const * input_file_path)
{
    static auto const get_op_priority = [](char const c) {
        if (c == '+') {
            return 1;
        }
        assert(c == '*');
        return 0;
    };
    static auto const parse_expression
        = [](aoc::StringView const & line) { return Expression::parse(line, get_op_priority); };

    auto const input{ aoc::read_file(input_file_path) };
    auto expressions{ aoc::StringView{ input }.iterate_transform(parse_expression, '\n') };
    auto const result{ std::transform_reduce(expressions.begin(),
                                             expressions.end(),
                                             number_t{},
                                             std::plus(),
                                             [](Expression & expression) { return expression.solve(); }) };
    return std::to_string(result);
}