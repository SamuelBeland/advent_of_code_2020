#include "utils.hpp"

#include <iostream>
#include <resources.hpp>

#include <map>

namespace
{
using rule_id_t = unsigned;

using value_t = unsigned;

constexpr value_t NONE = 0b00;
constexpr value_t A = 0b01;
constexpr value_t B = 0b10;
constexpr value_t ALL = A | B;

//==============================================================================
void print_rule(aoc::StringView const & rule,
                std::string & dest,
                std::map<rule_id_t, aoc::StringView> const & rules) noexcept
{
    auto const has_or{ rule.contains('|') };
    if (has_or) {
        dest.push_back('(');
    }

    auto const print_word = [&](aoc::StringView const & word) -> void {
        if (word == "a") {
            dest.push_back('a');
            return;
        }
        if (word == "b") {
            dest.push_back('b');
            return;
        }
        if (word == "|") {
            dest.push_back('|');
            return;
        }
        // is an index
        auto const index{ word.parse<rule_id_t>() };
        print_rule(rules.find(index)->second, dest, rules);
    };
    rule.iterate(print_word, ' ');

    if (has_or) {
        dest.push_back(')');
    }
}

//==============================================================================
void replace_all(std::string & string, aoc::StringView const & from, aoc::StringView const & to) noexcept
{
    for (auto pos{ string.find(from.cbegin(), 0, from.size()) }; pos < string.size();
         pos = string.find(from.cbegin(), pos + to.size(), from.size())) {
        string.replace(pos, from.size(), to.cbegin(), to.size());
    }
}

void crazy_filter(std::string & master_rule) noexcept
{
    std::vector<std::size_t> parens{};
    std::vector<std::size_t> ors{};
    for (size_t pos{}; pos < master_rule.size(); ++pos) {
        auto const & c{ master_rule[pos] };
        if (c == '(') {
            parens.push_back(pos);
            continue;
        }
        if (c == '|') {
            ors.push_back(pos);
        }
        if (c != ')') {
            continue;
        }
        // closing a paren: do some magic
        auto open_pos{ parens.back() };
        parens.pop_back();
        auto or_pos{ ors.back() };
        ors.pop_back();
        aoc::StringView lhs{ master_rule.data() + open_pos + 1, master_rule.data() + or_pos };
        aoc::StringView rhs{ master_rule.data() + or_pos + 1, master_rule.data() + pos };

        // unable to collapse : try moving chars out of paren at front
        std::size_t num_identical{};
        auto min_size{ std::min(lhs.size(), rhs.size()) };
        for (size_t i{}; i < min_size; ++i) {
            if (lhs[i] == '(') {
                break;
            }
            if (lhs[i] != rhs[i]) {
                break;
            }
            ++num_identical;
        }
        if (num_identical) {
            aoc::StringView const identical_string{ lhs.cbegin(), num_identical };
            auto const new_lhs{ lhs.remove_from_start(num_identical) };
            auto const new_rhs{ rhs.remove_from_start(num_identical) };
            auto new_string{ identical_string.to_std_string() };
            new_string.push_back('(');
            new_string.append(new_lhs.cbegin(), new_lhs.size());
            new_string.push_back('|');
            new_string.append(new_rhs.cbegin(), new_rhs.size());
            new_string.push_back(')');
            auto const old_size{ pos + 1 - open_pos };
            master_rule.replace(open_pos, old_size, new_string);
            pos -= old_size;
            pos += new_string.size();
            open_pos += num_identical;
            lhs = aoc::StringView{ master_rule.data() + open_pos + 1, master_rule.data() + or_pos };
            rhs = aoc::StringView{ master_rule.data() + or_pos + 1, master_rule.data() + pos };
            min_size -= num_identical;
        }

        // unable to collapse : try moving chars out of paren at back
        num_identical = 0;
        for (size_t i{}; i < min_size; ++i) {
            auto const & a{ lhs[lhs.size() - i - 1] };

            if (a == ')') {
                break;
            }

            auto const & b{ rhs[rhs.size() - i - 1] };
            if (a != b) {
                break;
            }
            ++num_identical;
        }
        if (num_identical) {
            aoc::StringView const identical_string{ lhs.cend() - num_identical, num_identical };
            auto const new_lhs{ lhs.remove_from_end(num_identical) };
            auto const new_rhs{ rhs.remove_from_end(num_identical) };
            std::string new_string{ "(" };
            new_string.append(new_lhs.cbegin(), new_lhs.size());
            new_string.push_back('|');
            new_string.append(new_rhs.cbegin(), new_rhs.size());
            new_string.push_back(')');
            new_string.append(identical_string.cbegin(), identical_string.size());
            auto const old_size{ pos + 1 - open_pos };
            master_rule.replace(open_pos, old_size, new_string);
            pos -= old_size;
            pos += new_string.size();
            pos -= num_identical;
            or_pos -= num_identical;
            lhs = aoc::StringView{ master_rule.data() + open_pos + 1, master_rule.data() + or_pos };
            rhs = aoc::StringView{ master_rule.data() + or_pos + 1, master_rule.data() + pos };
            min_size -= num_identical;
        }

        // no support for nested collapse right now
        if (lhs.contains('(') || rhs.contains('(')) {
            continue;
        }
        assert(lhs.size() == rhs.size());

        // try to collapse
        std::size_t mismatch{ std::string::npos };
        for (std::size_t i{}; i < lhs.size(); ++i) {
            if (lhs[i] != rhs[i]) {
                if (mismatch != std::string::npos) {
                    mismatch = std::string::npos;
                    break;
                }
                mismatch = i;
            }
        }
        if (mismatch != std::string::npos) {
            // single mismatch : do the collapse
            auto new_string{ lhs.to_std_string() };
            new_string[mismatch] = '*';
            auto const old_size{ pos + 1 - open_pos };
            master_rule.replace(open_pos, old_size, new_string);
            pos -= old_size;
            pos += new_string.size();
            continue;
        }

        // sort
        bool should_inverse{};
        for (size_t i{}; i < lhs.size(); ++i) {
            auto const & a{ lhs[i] };
            auto const & b{ rhs[i] };
            if (a == b) {
                continue;
            }
            if (a < b) {
                break;
            }
            should_inverse = true;
            break;
        }
        if (should_inverse) {
            auto const temp{ lhs.to_std_string() };
            master_rule.replace(open_pos + 1, rhs.size(), rhs.cbegin(), rhs.size());
            master_rule.replace(or_pos + 1, temp.size(), temp);
        }
    }

    bool is_nested_paren{};
    std::vector<std::string> patterns{};
    for (int i{}; i < 8; ++i) {
        for (std::size_t pos{}; pos < master_rule.size(); ++pos) {
            auto const & c{ master_rule[pos] };
            if (c == '(') {
                parens.push_back(pos);
                is_nested_paren = false;
                continue;
            }
            if (c != ')') {
                continue;
            }

            if (is_nested_paren) {
                continue;
            }
            is_nested_paren = true;

            auto const open_pos{ parens.back() };
            parens.pop_back();
            auto const old_size{ pos + 1 - open_pos };
            auto pattern{ aoc::StringView{ master_rule.data() + open_pos + 1, old_size }.to_std_string() };
            auto it{ std::find(patterns.cbegin(), patterns.cend(), pattern) };
            if (it == patterns.cend()) {
                it = patterns.insert(it, std::move(pattern));
            }
            auto const index{ std::distance(patterns.cbegin(), it) };
            std::string new_string{};
            new_string.push_back('p');
            new_string += std::to_string(index);
            master_rule.replace(open_pos, it->size(), new_string);
            pos += new_string.size();
            pos -= old_size;
        }
    }
}

} // namespace

//==============================================================================
std::string day_19_a(char const * input_file_path)
{
    // parse all rules
    auto const input{ aoc::read_file(input_file_path) };
    aoc::StringView rules_string{};
    aoc::StringView messages_string{};
    aoc::StringView{ input }.scan("{}\n\n{}", rules_string, messages_string);
    std::map<rule_id_t, aoc::StringView> rules{};
    auto const parse_and_insert_rule = [&](aoc::StringView const & line) -> void {
        std::pair<rule_id_t, aoc::StringView> rule;
        line.scan("{}: {}", rule.first, rule.second);
        rules.insert(rule);
    };
    rules_string.iterate(parse_and_insert_rule, '\n');
    // replace "a" with a and "b" with b
    for (auto & rule : rules) {
        if (rule.second == "\"a\"") {
            rule.second = "a";
            continue;
        }
        if (rule.second == "\"b\"") {
            rule.second = "b";
        }
    }
    // print master_rule
    std::string master_rule{};
    print_rule(rules.begin()->second, master_rule, rules);

    // replacing time!
    crazy_filter(master_rule);

    std::cout << master_rule << "\n\n";

    return "";
}

//==============================================================================
std::string day_19_b(char const * input_file_path)
{
    return "";
}