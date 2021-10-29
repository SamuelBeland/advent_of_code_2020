//-- - Day 6: Custom Customs-- -
// As your flight approaches the regional airport where you'll switch to a much larger plane, customs declaration forms
// are distributed to the passengers.
//
// The form asks a series of 26 yes - or -no questions marked a through z.All you need to do is identify the questions
// for which anyone in your group answers "yes".Since your group is just you, this doesn't take very long.
//
// However, the person sitting next to you seems to be experiencing a language barrierand asks if you can help.For each
// of the people in their group, you write down the questions for which they answer "yes", one per line.For example :
//
// abcx
// abcy
// abcz
//
// In this group, there are 6 questions to which anyone answered "yes" : a, b, c, x, y, and z. (Duplicate answers to the
// same question don't count extra; each question counts at most once.)
//
//    Another group asks for your help, then another, and eventually you've collected answers from every group on the
//    plane (your puzzle input). Each group's answers are separated by a blank line, and within each group, each
//    person's answers are on a single line. For example:
//
//    abc
//
//    a
//    b
//    c
//
//    ab
//    ac
//
//    a
//    a
//    a
//    a
//
//    b
//
//    This list represents answers from five groups :
//
// The first group contains one person who answered "yes" to 3 questions : a, b, and c.
// The second group contains three people; combined, they answered "yes" to 3 questions: a, b, and c.
// The third group contains two people; combined, they answered "yes" to 3 questions: a, b, and c.
// The fourth group contains four people; combined, they answered "yes" to only 1 question, a.
// The last group contains one person who answered "yes" to only 1 question, b.
// In this example, the sum of these counts is 3 + 3 + 3 + 1 + 1 = 11.
//
// For each group, count the number of questions to which anyone answered "yes".What is the sum of those counts ?
//
//-- - Part Two-- -
// As you finish the last group's customs declaration, you notice that you misread one word in the instructions:
//
// You don't need to identify the questions to which anyone answered "yes"; you need to identify the questions to which
// everyone answered "yes"!
//
// Using the same example as above :
//
// abc
//
// a
// b
// c
//
// ab
// ac
//
// a
// a
// a
// a
//
// b
//
// This list represents answers from five groups :
//
// In the first group, everyone(all 1 person) answered "yes" to 3 questions : a, b, and c.
// In the second group, there is no question to which everyone answered "yes".
// In the third group, everyone answered yes to only 1 question, a.Since some people did not answer "yes" to b or c,
// they don't count. In the fourth group, everyone answered yes to only 1 question, a. In the fifth group, everyone(all
// 1 person) answered "yes" to 1 question, b. In this example, the sum of these counts is 3 + 0 + 1 + 1 + 1 = 6.
//
// For each group, count the number of questions to which everyone answered "yes".What is the sum of those counts ?

#include "utils.hpp"
#include <resources.hpp>

namespace
{
//==============================================================================
auto get_unique_answers(std::string_view const & group)
{
    std::string group_copy{ group };
    aoc::sort(group_copy);
    auto const first_alpha_char{ aoc::find_if(group_copy, [](char const & character) { return character != '\n'; }) };
    auto const unique_end{ std::unique(first_alpha_char, group_copy.end()) };
    auto const size{ unique_end - first_alpha_char };
    return size;
}

//==============================================================================
auto get_consensus_answers(std::string_view const & group)
{
    auto const persons{ aoc::split(group) };
    auto const & candidates{ persons.front() };
    auto const every_person_has_candidate = [&persons](char const candidate) {
        return std::all_of(persons.cbegin() + 1, persons.cend(), [candidate](std::string_view const & person) {
            return person.find(candidate, 0) != std::string_view::npos;
        });
    };
    auto const count{ aoc::count_if(candidates, every_person_has_candidate) };
    return count;
}

} // namespace

//==============================================================================
std::string day_6_a(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    auto const groups{ aoc::split(input, "\n\n") };
    auto const sum_of_group_sums{
        aoc::transform_reduce(groups, std::string::difference_type(0), get_unique_answers, std::plus())
    };
    return std::to_string(sum_of_group_sums);
}

//==============================================================================
std::string day_6_b(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    auto const groups{ aoc::split(input, "\n\n") };
    auto const sum_of_group_sums{
        aoc::transform_reduce(groups, std::string::difference_type(0), get_consensus_answers, std::plus())
    };
    return std::to_string(sum_of_group_sums);
}
