#include "utils.hpp"
#include <resources.hpp>

#include <map>

namespace
{
auto parse_rule(aoc::StringView const & line) noexcept
{
    std::pair<std::string, std::string> result;
    line.scan("{}: {}", result.first, result.second);
    return result;
}

std::string
    replace_word(aoc::StringView const & string, aoc::StringView const & old, aoc::StringView const & new_) noexcept
{
    std::string result{};
    auto const add_to_string_or_replace = [&](aoc::StringView const & word) {
        if (!result.empty()) {
            result.push_back(' ');
        }
        if (word != old) {
            result.append(word.cbegin(), word.size());
            return;
        }
        result.append(new_.cbegin(), new_.size());
    };
    string.iterate(add_to_string_or_replace, ' ');
    return result;
}

} // namespace

//==============================================================================
std::string day_19_a(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    aoc::StringView rules_string{};
    aoc::StringView messages_string{};
    aoc::StringView{ input }.scan("{}\n\n{}", rules_string, messages_string);
    std::map<std::string, std::string> rules{};
    auto const parse_and_add_rule = [&](aoc::StringView const & rule) { rules.insert(parse_rule(rule)); };
    rules_string.iterate(parse_and_add_rule, '\n');
    for (auto & rule : rules) {
        if (rule.second == "\"a\"") {
            rule.second = "a";
        } else if (rule.second == "\"b\"") {
            rule.second = "b";
        }
    }
    while (rules.size() > 1) {
        // TODO : does not seems to always start with zero!
        auto const rule_to_collapse{ rules.extract(std::next(rules.cbegin())) };
        for (auto & rule : rules) {
            rule.second = replace_word(rule.second, rule_to_collapse.key(), rule_to_collapse.mapped());
        }
    }

    return "";
}

//==============================================================================
std::string day_19_b(char const * input_file_path)
{
    return "";
}