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

#include "utils.hpp"
#include <resources.hpp>

#include <algorithm>
#include <iterator>

using number_t = uint64_t;

//==============================================================================
struct Range {
    number_t min;
    number_t max;

    //==============================================================================
    [[nodiscard]] bool contains(number_t const value) const { return value >= min && value <= max; }
    //==============================================================================
    [[nodiscard]] bool contains(Range const & other) const { return contains(other.min) || contains(other.max); }
    //==============================================================================
    [[nodiscard]] Range merge(Range const & other) const
    {
        return Range{ min < other.min ? min : other.min, max > other.max ? max : other.max };
    }
};

//==============================================================================
struct Rule {
    std::string name;
    Range low_range;
    Range high_range;

    //==============================================================================
    [[nodiscard]] bool contains(number_t const value) const
    {
        return low_range.contains(value) || high_range.contains(value);
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
    result.reserve(lines.size());
    std::transform(lines.cbegin(), lines.cend(), std::back_inserter(result), [](std::string_view const & line) {
        return Rule::from_string(line);
    });
    return result;
}

//==============================================================================
using Ticket = std::vector<number_t>;

//==============================================================================
std::vector<Ticket> parse_tickets(std::string_view const string)
{
    auto const lines{ split(string) };
    std::vector<Ticket> result{ lines.size() };
    transform(lines, result, [](std::string_view const & line) { return scan_number_list<number_t>(line, ','); });
    return result;
}

//==============================================================================
number_t get_ticket_scanning_error_rate(std::vector<Ticket> const & tickets, std::vector<Rule> const & rules)
{
    auto const is_valid_entry = [&](number_t const & value) {
        return all_of(rules, [&](Rule const & rule) { return rule.contains(value); });
    };

    auto const accumulate_invalid_entries = [&](Ticket const & ticket) {
        return reduce(ticket, number_t{}, [&](number_t const & error_rate_for_ticket, number_t const & value) {
            return is_valid_entry(value) ? error_rate_for_ticket : error_rate_for_ticket + value;
        });
    };

    return reduce(tickets, number_t{}, [&](number_t const & error_rate, Ticket const & ticket) {
        return error_rate + accumulate_invalid_entries(ticket);
    });
}

//==============================================================================
struct Day_16_Data {
    std::vector<Rule> rules;
    Ticket my_ticket;
    std::vector<Ticket> nearby_tickets;
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
bool is_valid_ticket(Ticket const & ticket, std::vector<Rule> const & rules)
{
    auto const is_valid_field = [&](number_t const & number) {
        return any_of(rules, [&](Rule const & rule) { return rule.contains(number); });
    };

    return all_of(ticket, is_valid_field);
}

//==============================================================================

//==============================================================================
std::string day_16_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const info{ Day_16_Data::from_string(input) };
    auto const error_rate{ get_ticket_scanning_error_rate(info.nearby_tickets, info.rules) };

    return std::to_string(error_rate);
}

//==============================================================================
std::string day_16_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const data{ Day_16_Data::from_string(input) };
    return "coucou";
}