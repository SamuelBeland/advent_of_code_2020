#include "utils.hpp"
#include <resources.hpp>

#include <variant>

namespace
{
using number_t = uint64_t;

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
};

Operator * solve_operator(Operator * op)
{
    assert(op != nullptr);
    assert(op->lhs != nullptr);
    assert(op->rhs != nullptr);
    assert(op->c == '+' || op->c == '*');

    op->lhs->value = op->c == '+' ? op->lhs->value + op->rhs->value : op->lhs->value * op->rhs->value;
    Operator * result{};
    if (op->rhs->next_op) {
        op->rhs->next_op->lhs = op->lhs;
        result = op->rhs->next_op;
        delete op->rhs;
        delete op;
    }
    return result;
}

struct Expression {
    std::unique_ptr<Term> first_term;
    unsigned max_priority;

    number_t solve()
    {
        assert(first_term);
        assert(first_term->previous_op == nullptr);
        do {
            Operator * op{ first_term->next_op };
            while (op != nullptr) {
                if (op->priority == max_priority) {
                    op = solve_operator(op);
                } else {
                    op = op->rhs->next_op;
                }
            }
        } while (max_priority-- > 0);
        return first_term->value;
    }

    template<typename OperatorPriorityFunc>
    static Expression parse(aoc::StringView const & string, OperatorPriorityFunc const & operator_priority_func)
    {
        Expression result{ std::make_unique<Term>(), 0 };
        unsigned current_priority{};
        auto const * term_begin{ string.cbegin() };

        for (auto const c : string) {
            switch (c) {
            case '(':
                ++current_priority;
                continue;
            case ')':
            }
        }
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