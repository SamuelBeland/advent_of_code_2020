#include "utils.hpp"
#include <resources.hpp>

#include <regex>

namespace
{
constexpr aoc::StringView A = "\"a\"";
constexpr aoc::StringView B = "\"b\"";

//==============================================================================
struct Rule {
    std::vector<std::size_t> rules;
    std::vector<std::size_t> alt_rules;

    //==============================================================================
    static Rule parse(aoc::StringView const & string) noexcept
    {
        assert(string != A && string != B);

        Rule result{};
        auto * rules{ &result.rules };
        auto const add_index = [&](aoc::StringView const & word) {
            if (word == "|") {
                assert(rules == &result.rules);
                rules = &result.alt_rules;
                return;
            }
            rules->push_back(word.parse<std::size_t>());
        };
        string.iterate(add_index, ' ');
        return result;
    }
};

//==============================================================================
struct Rules {
    std::vector<Rule> rules{};
    std::size_t a_index{};
    std::size_t b_index{};

    //==============================================================================
    [[nodiscard]] static Rules parse(aoc::StringView const & string) noexcept
    {
        using rule_with_index_t = std::pair<std::size_t, Rule>;

        Rules result{};
        auto const parse_line = [&](aoc::StringView const & line) {
            rule_with_index_t item;
            aoc::StringView rule;
            line.scan("{}: {}", item.first, rule);
            if (rule == A) {
                result.a_index = item.first;
            } else if (rule == B) {
                result.b_index = item.first;
            } else {
                item.second = Rule::parse(rule);
            }
            return item;
        };
        auto temp{ string.iterate_transform(parse_line, '\n') };
        std::sort(temp.begin(), temp.end(), [](rule_with_index_t const & a, rule_with_index_t const & b) {
            return a.first < b.first;
        });
        result.rules.resize(temp.size());
        auto inserter{ result.rules.begin() };
        for (auto & it : temp) {
            *inserter++ = std::move(it.second);
        }
        return result;
    }

    //==============================================================================
    [[nodiscard]] std::regex to_regex() const noexcept
    {
        std::string expression{};
        print_to_string(0, expression);
        return std::regex{ expression, std::regex::ECMAScript | std::regex::optimize };
    }

private:
    //==============================================================================
    void print_to_string(std::size_t const index, std::string & dest) const noexcept
    {
        auto const & rule{ rules[index] };

        if (index == a_index) {
            dest.push_back('a');
            return;
        }
        if (index == b_index) {
            dest.push_back('b');
            return;
        }

        auto const has_or{ !rule.alt_rules.empty() };
        if (has_or) {
            dest.push_back('(');
        }

        for (auto const & sub_index : rule.rules) {
            print_to_string(sub_index, dest);
        }
        if (!has_or) {
            return;
        }

        dest.push_back('|');
        for (auto const & sub_index : rule.alt_rules) {
            print_to_string(sub_index, dest);
        }

        dest.push_back(')');
    }
};

//==============================================================================
[[nodiscard]] auto count_matches(std::regex const & expression, std::vector<aoc::StringView> const & messages) noexcept
{
    auto const matches = [&](aoc::StringView const & message) -> bool {
        return std::regex_match(message.cbegin(), message.cend(), expression);
    };

    return std::count_if(messages.cbegin(), messages.cend(), matches);
}

} // namespace

//==============================================================================
std::string day_19_a(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    aoc::StringView rules_string{};
    aoc::StringView messages_string{};
    aoc::StringView{ input }.scan("{}\n\n{}", rules_string, messages_string);
    auto const rule{ Rules::parse(rules_string).to_regex() };
    auto const messages{ messages_string.split('\n') };
    auto const num_matches{ count_matches(rule, messages) };

    return std::to_string(num_matches);
}

//==============================================================================
std::string day_19_b(char const * input_file_path)
{
    return "";
}