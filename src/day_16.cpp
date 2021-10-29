//-- - Day 16: Ticket Translation-- -
//
// As you're walking to yet another connecting flight, you realize that one of the legs of your re-routed trip coming up
// is on a high-speed train. However, the train ticket you were given is in a language you don't understand.You should
// probably figure out what it says before you get to the train station after the next flight.
//
// Unfortunately, you can't actually read the words on the ticket. You can, however, read the numbers, and so you figure
// out the fields these tickets must have and the valid ranges for values in those fields.
//
// You collect the rules for ticket fields, the numbers on your ticket, and the numbers on other nearby tickets for the
// same train service(via the airport security cameras) together into a single document you can reference(your puzzle
// input).
//
// The rules for ticket fields specify a list of fields that exist somewhere on the ticketand the valid ranges of values
// for each field.For example, a rule like class : 1 - 3 or 5 - 7 means that one of the fields in every ticket is named
// class and can be any value in the ranges 1 - 3 or 5 - 7 (inclusive, such that 3 and 5 are both valid in this field,
// but 4 is not).
//
// Each ticket is represented by a single line of comma - separated values.The values are the numbers on the ticket in
// the order they appear; every ticket has the same format.
//
// For example, consider this ticket:
//
//.--------------------------------------------------------.
//| ? ? ? ? : 101 ? ? ? ? ? : 102 ? ? ? ? ? ? ? ? ? ? : 103 ? ? ? : 104 |
//|                                                        |
//| ? ? : 301 ? ? : 302 ? ? ? ? ? ? ? : 303 ? ? ? ? ? ? ? |
//| ? ? : 401 ? ? : 402 ? ? ? ? ? ? ? ? : 403 ? ? ? ? ? ? ? ? ? |
//'--------------------------------------------------------'
//
// Here, ? represents text in a language you don't understand. This ticket might be represented as
// 101,102,103,104,301,302,303,401,402,403; of course, the actual train tickets you're looking at are much more
// complicated.In any case, you've extracted just the numbers in such a way that the first number is always the same
// specific field, the second number is always a different specific field, and so on - you just don't know what each
// position actually means!
//
// Start by determining which tickets are completely invalid; these are tickets that contain values which aren't valid
// for any field. Ignore your ticket for now.
//
// For example, suppose you have the following notes :
//
// class : 1 - 3 or 5 - 7
// row : 6 - 11 or 33 - 44
// seat : 13 - 40 or 45 - 50
//
// your ticket :
// 7, 1, 14
//
// nearby tickets :
//
// 7, 3, 47
// 40, 4, 50
// 55, 2, 20
// 38, 6, 12
//
// It doesn't matter which position corresponds to which field; you can identify invalid nearby tickets by considering
// only whether tickets contain values that are not valid for any field. In this example, the values on the first nearby
// ticket are all valid for at least one field. This is not true of the other three nearby tickets: the values 4, 55,
// and 12 are are not valid for any field. Adding together all of the invalid values produces your ticket scanning error
// rate: 4 + 55 + 12 = 71.
//
// Consider the validity of the nearby tickets you scanned.
//
// What is your ticket scanning error rate ?
//
// Your puzzle answer was 19060.
//
// The first half of this puzzle is complete!It provides one gold star : *
//
//-- - Part Two-- -
//
// Now that you've identified which tickets contain invalid values, discard those tickets entirely. Use the remaining
// valid tickets to determine which field is which.
//
// Using the valid ranges for each field, determine what order the fields appear on the tickets.The order is consistent
// between all tickets : if seat is the third field, it is the third field on every ticket, including your ticket.
//
// For example, suppose you have the following notes :
//
// class : 0 - 1 or 4 - 19
// row : 0 - 5 or 8 - 19
// seat : 0 - 13 or 16 - 19
//
// your ticket :
// 11, 12, 13
//
// nearby tickets :
//
// 3, 9, 18
// 15, 1, 5
// 5, 14, 9
//
// Based on the nearby tickets in the above example, the first position must be row, the second position must be class,
// and the third position must be seat; you can conclude that in your ticket, class is 12, row is 11, and seat is 13.
//
// Once you work out which field is which, look for the six fields on your ticket that start with the word
// departure.What do you get if you multiply those six values together ?
//
// Your puzzle answer was 953713095011.

#include "utils.hpp"
#include <resources.hpp>

#include <algorithm>
#include <cassert>
#include <iterator>

namespace
{
using number_t = uint64_t;

//==============================================================================
struct Range {
    number_t min;
    number_t max;

    //==============================================================================
    [[nodiscard]] constexpr bool contains(number_t const value) const { return value >= min && value <= max; }
};

//==============================================================================
struct Rule {
    std::string name;
    Range low_range;
    Range high_range;

    //==============================================================================
    [[nodiscard]] constexpr bool contains(number_t const value) const
    {
        return low_range.contains(value) || high_range.contains(value);
    }
    [[nodiscard]] bool contains_all(std::vector<number_t> const & values) const
    {
        assert(std::is_sorted(values.cbegin(), values.cend()));

        if (!low_range.contains(values.front()) || !high_range.contains(values.back())) {
            return false;
        }

        auto const end_of_low_range{ aoc::upper_bound(values, low_range.max) };
        auto const start_of_high_range{ std::lower_bound(end_of_low_range, values.cend(), high_range.min) };

        // no values between low_range.max and high_range.min
        return end_of_low_range == start_of_high_range;
    }
    [[nodiscard]] bool is_departure() const { return name.find("departure") == 0; }
    //==============================================================================
    [[nodiscard]] static Rule from_string(std::string_view const & line)
    {
        Rule result{};
        scan(line,
             "{}: {}-{} or {}-{}",
             result.name,
             result.low_range.min,
             result.low_range.max,
             result.high_range.min,
             result.high_range.max);
        return result;
    }
};

//==============================================================================
[[nodiscard]] std::vector<Rule> parse_rules(std::string_view const & string)
{
    auto const lines{ split(string) };
    std::vector<Rule> result{};
    result.resize(lines.size());
    aoc::transform(lines, result, Rule::from_string);

    return result;
}

//==============================================================================
using Ticket = std::vector<number_t>;

//==============================================================================
std::vector<Ticket> parse_tickets(std::string_view const string)
{
    auto const lines{ split(string) };
    std::vector<Ticket> result{ lines.size() };
    aoc::transform(lines, result, [](std::string_view const & line) { return scan_number_list<number_t>(line, ','); });
    return result;
}

//==============================================================================
number_t get_ticket_scanning_error_rate(std::vector<Ticket> const & tickets, std::vector<Rule> const & rules)
{
    auto const is_invalid_entry = [&](number_t const & value) {
        return aoc::all_of(rules, [&](Rule const & rule) { return !rule.contains(value); });
    };

    std::vector<number_t> invalid_entries{};

    auto inserter{ std::back_inserter(invalid_entries) };
    for (auto const & ticket : tickets) {
        std::copy_if(ticket.cbegin(), ticket.cend(), std::back_inserter(invalid_entries), is_invalid_entry);
    }

    return aoc::reduce(invalid_entries, number_t{}, std::plus());
}

//==============================================================================
std::vector<size_t> build_index_list(size_t const number_of_items)
{
    std::vector<size_t> index_list{};
    index_list.resize(number_of_items);
    std::iota(std::begin(index_list), std::end(index_list), size_t{});
    return index_list;
}

//==============================================================================
struct Solved_Field {
    size_t rule_index;
    size_t field_index;
};

//==============================================================================
std::vector<Solved_Field> deduce(std::vector<Ticket> const & tickets, std::vector<Rule> const & rules);

//==============================================================================
struct Day_16_Data {
    std::vector<Rule> rules;
    Ticket my_ticket;
    std::vector<Ticket> nearby_tickets;
    //==============================================================================
    [[nodiscard]] number_t get_departure_product() const
    {
        auto const departure_rule_indexes{ get_departure_rule_indexes() };
        auto const solution{ deduce(nearby_tickets, rules) };

        auto const get_field_index_from_rule_index = [&](size_t const rule_index) {
            auto const solved_field_it{ aoc::find_if(solution, [&](Solved_Field const & solved_field) {
                return solved_field.rule_index == rule_index;
            }) };
            assert(solved_field_it != std::cend(solution));
            return solved_field_it->field_index;
        };

        auto const result{ aoc::transform_reduce(
            departure_rule_indexes,
            number_t{ 1 },
            [&](size_t const departure_rule) {
                auto const field_index{ get_field_index_from_rule_index(departure_rule) };
                return my_ticket[field_index];
            },
            std::multiplies()) };

        return result;
    }
    //==============================================================================
    [[nodiscard]] std::vector<size_t> get_departure_rule_indexes() const
    {
        auto departure_rule_indexes{ build_index_list(rules.size()) };
        aoc::remove_if(departure_rule_indexes,
                       [&](size_t const rule_index) { return !rules[rule_index].is_departure(); });
        return departure_rule_indexes;
    }
    //==============================================================================
    static Day_16_Data from_string(std::string_view const & string)
    {
        std::string_view ticket_fields_string;
        std::string_view my_ticket_values_string;
        std::string_view nearby_tickets_values_string;
        scan(string,
             "{}\n\nyour ticket:\n{}\n\nnearby tickets:\n{}",
             ticket_fields_string,
             my_ticket_values_string,
             nearby_tickets_values_string);

        return Day_16_Data{ parse_rules(ticket_fields_string),
                            scan_number_list<number_t>(my_ticket_values_string, ','),
                            parse_tickets(nearby_tickets_values_string) };
    }
};

//==============================================================================
std::vector<Ticket> remove_invalid_tickets(std::vector<Ticket> const & tickets, std::vector<Rule> const & rules)
{
    auto const is_valid_ticket = [&](Ticket const & ticket) {
        auto const is_valid_field = [&](number_t const & number) {
            return aoc::any_of(rules, [&](Rule const & rule) { return rule.contains(number); });
        };

        return aoc::all_of(ticket, is_valid_field);
    };

    std::vector<Ticket> valid_tickets{};
    valid_tickets.reserve(tickets.size());
    std::copy_if(std::cbegin(tickets), std::cend(tickets), std::back_inserter(valid_tickets), is_valid_ticket);
    return valid_tickets;
}

//==============================================================================
std::vector<number_t> collect_field_samples(std::vector<Ticket> const & tickets, size_t const index)
{
    std::vector<number_t> field_samples{};
    field_samples.reserve(tickets.size());
    std::transform(std::cbegin(tickets),
                   std::cend(tickets),
                   std::back_inserter(field_samples),
                   [&](Ticket const & ticket) {
                       assert(ticket.size() > index);
                       return ticket[index];
                   });
    aoc::sort(field_samples);
    return field_samples;
}

//==============================================================================
struct Unsolved_Field {
    std::vector<number_t> samples;
    std::vector<size_t> rule_index_candidates;
};

//==============================================================================
std::vector<Unsolved_Field> construct_unsolved_fields(std::vector<Ticket> const & tickets)
{
    std::vector<Unsolved_Field> unsolved_fields{};
    assert(!tickets.empty());
    auto const number_of_fields{ tickets.front().size() };
    assert(aoc::all_of(tickets, [&](Ticket const & ticket) { return ticket.size() == number_of_fields; }));
    unsolved_fields.reserve(number_of_fields);
    auto const index_list{ build_index_list(number_of_fields) };
    for (size_t field_index{}; field_index < number_of_fields; ++field_index) {
        unsolved_fields.emplace_back(Unsolved_Field{ collect_field_samples(tickets, field_index), index_list });
    }
    return unsolved_fields;
}

//==============================================================================
size_t register_solved_rule(size_t const solved_rule_index,
                            size_t const solved_field_index,
                            std::vector<Unsolved_Field> & unsolved_fields)
{
    auto const number_of_fields{ unsolved_fields.size() };
    size_t number_of_solved_fields{ 1 };
    for (size_t field_index{}; field_index < number_of_fields; ++field_index) {
        if (field_index == solved_field_index) {
            // don't erase yourself
            continue;
        }
        auto & rule_index_candidates{ unsolved_fields[field_index].rule_index_candidates };
        if (rule_index_candidates.size() == 1) {
            // already solved
            continue;
        }

        auto const rule_index_to_erase{
            std::find(std::cbegin(rule_index_candidates), std::cend(rule_index_candidates), solved_rule_index)
        };

        if (rule_index_to_erase == std::cend(rule_index_candidates)) {
            // nothing to erase
            continue;
        }

        rule_index_candidates.erase(rule_index_to_erase);

        if (rule_index_candidates.size() == 1) {
            number_of_solved_fields
                += register_solved_rule(rule_index_candidates.front(), field_index, unsolved_fields);
        }
    }

    return number_of_solved_fields;
}

//==============================================================================
std::vector<Solved_Field> deduce(std::vector<Ticket> const & tickets, std::vector<Rule> const & rules)
{
    auto const valid_tickets{ remove_invalid_tickets(tickets, rules) };
    auto unsolved_fields{ construct_unsolved_fields(valid_tickets) };
    auto const number_of_fields{ unsolved_fields.size() };
    assert(number_of_fields == rules.size());

    size_t number_of_solved_fields{};

    while (number_of_solved_fields < number_of_fields) {
        for (size_t field_index{}; field_index < number_of_fields; ++field_index) {
            auto & unsolved_field{ unsolved_fields[field_index] };

            if (unsolved_field.rule_index_candidates.size() == 1) {
                // already solved
                continue;
            }

            auto const is_invalid_rule_index_candidate = [&](size_t const rule_index_candidate) {
                auto const & candidate_rule{ rules[rule_index_candidate] };
                return !candidate_rule.contains_all(unsolved_field.samples);
            };

            auto & rule_index_candidates{ unsolved_field.rule_index_candidates };
            aoc::remove_if(rule_index_candidates, is_invalid_rule_index_candidate);
            if (rule_index_candidates.size() > 1) {
                // not solved yet
                continue;
            }
            // solved
            assert(rule_index_candidates.size() == 1);

            // add to solved
            number_of_solved_fields
                += register_solved_rule(rule_index_candidates.front(), field_index, unsolved_fields);
        }
    }

    // compile data
    std::vector<Solved_Field> solved_fields{};
    solved_fields.reserve(number_of_fields);

    for (size_t field_index{}; field_index < number_of_fields; ++field_index) {
        solved_fields.emplace_back(
            Solved_Field{ unsolved_fields[field_index].rule_index_candidates.front(), field_index });
    }

    return solved_fields;
}

} // namespace

//==============================================================================
std::string day_16_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const data{ Day_16_Data::from_string(input) };
    auto const error_rate{ get_ticket_scanning_error_rate(data.nearby_tickets, data.rules) };

    return std::to_string(error_rate);
}

//==============================================================================
std::string day_16_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const data{ Day_16_Data::from_string(input) };
    auto const departure_product{ data.get_departure_product() };

    return std::to_string(departure_product);
}