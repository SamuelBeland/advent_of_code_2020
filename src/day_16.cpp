#include <algorithm>
#include <numeric>

#include "utils.hpp"
#include <resources.hpp>

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
struct Field_Rule {
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
    [[nodiscard]] static Field_Rule from_string(std::string_view const & line)
    {
        Field_Rule result{};
        scan(line,
             "{}: {}-{} or {}-{}",
             result.name,
             result.low_range.min,
             result.low_range.max,
             result.high_range.min,
             result.high_range.max);
        return result;
    }
    //==============================================================================
    [[nodiscard]] static std::vector<Field_Rule> from_list_string(std::string_view const & string)
    {
        auto const lines{ split(string) };
        std::vector<Field_Rule> result{};
        result.reserve(lines.size());
        for (auto const & line : lines) {
            result.emplace_back(from_string(line));
        }
        return result;
    }
};

//==============================================================================
using Ticket = std::vector<number_t>;

//==============================================================================
std::vector<Ticket> parse_ticket_values(std::string_view const string)
{
    auto const lines{ split(string) };
    std::vector<Ticket> result{ lines.size() };
    transform(lines, result, [](std::string_view const & line) { return scan_list<number_t>(line, ','); });
    return result;
}

//==============================================================================
void flatten_range(std::vector<Range> & non_overlapping_ranges, Range const & new_range)
{
    auto const colliding_range{ find_if(non_overlapping_ranges,
                                        [new_range](Range const & range) { return range.contains(new_range); }) };

    if (colliding_range != non_overlapping_ranges.end()) {
        auto const merged_new_range{ colliding_range->merge(new_range) };
        non_overlapping_ranges.erase(colliding_range);
        flatten_range(non_overlapping_ranges, merged_new_range);
    } else {
        non_overlapping_ranges.push_back(new_range);
    }
}

//==============================================================================
std::vector<Range> get_valid_ranges(std::vector<Field_Rule> const & field_rules)
{
    return reduce(field_rules,
                  std::vector<Range>{},
                  [](std::vector<Range> & valid_ranges, Field_Rule const & rule) -> std::vector<Range> & {
                      flatten_range(valid_ranges, rule.low_range);
                      flatten_range(valid_ranges, rule.high_range);
                      return valid_ranges;
                  });
}

//==============================================================================
[[nodiscard]] std::vector<Ticket> get_valid_tickets(std::vector<Ticket> const & tickets,
                                                    std::vector<Field_Rule> const & field_rules)
{
    auto const valid_ranges{ get_valid_ranges(field_rules) };
    auto candidates{ tickets };
    remove_if(candidates, [&valid_ranges](Ticket const & ticket) {
        return any_of(ticket, [&valid_ranges](number_t const value) {
            return all_of(valid_ranges, [value](Range const & range) { return !range.contains(value); });
        });
    });
    return candidates;
}

//==============================================================================
std::vector<number_t> get_invalid_numbers(std::vector<Ticket> const & tickets,
                                          std::vector<Field_Rule> const & field_rules)
{
    auto const valid_ranges{ get_valid_ranges(field_rules) };
    std::vector<number_t> invalid_numbers{};

    // auto const test = std::reduce()

    auto const add_invalid_numbers = [&invalid_numbers, &valid_ranges](std::vector<number_t> const & values) {
        for (auto const number : values) {
            if (all_of(valid_ranges, [number](Range const & valid_range) { return !valid_range.contains(number); })) {
                invalid_numbers.emplace_back(number);
            }
        }
    };

    std::for_each(tickets.cbegin(), tickets.cend(), add_invalid_numbers);

    return invalid_numbers;
}

//==============================================================================
number_t get_ticket_scanning_error_rate(std::vector<Ticket> const & tickets,
                                        std::vector<Field_Rule> const & field_rules)
{
    auto const invalid_numbers{ get_invalid_numbers(tickets, field_rules) };

    auto const result{ reduce(invalid_numbers, number_t{}, std::plus{}) };
    return result;
}

//==============================================================================
using Prospects = std::vector<size_t>;
using Field_Samples = std::vector<number_t>;

//==============================================================================
std::vector<Prospects> fill_prospect_list(size_t const number_of_fields)
{
    std::vector<Prospects> results{ number_of_fields };
    results.front().resize(number_of_fields);
    iota(results.front(), size_t{});
    std::fill(results.begin() + 1, results.end(), results.front());
    return results;
}

//==============================================================================
void deduce(std::vector<Prospects>::const_iterator const to_remove_begin,
            std::vector<Prospects>::const_iterator const to_remove_end,
            std::vector<Prospects>::iterator const to_remove_from_begin,
            std::vector<Prospects>::iterator const to_remove_from_end)
{
    auto const remove_elements_from_prospects = [](std::vector<Prospects>::const_iterator const to_remove_begin,
                                                   std::vector<Prospects>::const_iterator const to_remove_end,
                                                   Prospects & to_remove_from) {
        remove_if(to_remove_from, [&to_remove_begin, &to_remove_end](size_t const & to_remove_from_value) {
            return std::none_of(to_remove_begin, to_remove_end, [&to_remove_from_value](Prospects const & to_remove) {
                assert(to_remove.size() == 1);
                return to_remove.front() == to_remove_from_value;
            });
        });
    };

    for (auto it{ to_remove_from_begin }; it != to_remove_from_end; ++it) {
        remove_elements_from_prospects(to_remove_begin, to_remove_end, *it);
    }

    auto const new_to_remove_from_begin{ std::partition(
        to_remove_from_begin,
        to_remove_from_end,
        [](Prospects const & prospects) { return prospects.size() == 1; }) };

    if (new_to_remove_from_begin != to_remove_from_begin) {
        deduce(to_remove_end, new_to_remove_from_begin, new_to_remove_from_begin, to_remove_from_end);
    }
}

//==============================================================================
number_t get_departure_product(std::vector<Ticket> const & tickets,
                               std::vector<Field_Rule> const & field_rules,
                               std::vector<number_t> const & my_ticket)
{
    // get valid, rotated data
    auto const organize_ticket_values_by_fields = [](std::vector<Ticket> const & tickets) {
        std::vector<Field_Samples> result{};
        auto const number_of_fields{ tickets.front().size() };
        auto const number_of_tickets{ tickets.size() };
        result.resize(number_of_fields);
        for (size_t field_index{}; field_index < number_of_fields; ++field_index) {
            result[field_index].resize(number_of_tickets);
            transform(tickets, result[field_index], [field_index](std::vector<number_t> const & ticket_values) {
                return ticket_values[field_index];
            });
        }
        return result;
    };

    auto const samples{ organize_ticket_values_by_fields(get_valid_tickets(tickets, field_rules)) };
    auto prospect_list{ fill_prospect_list(field_rules.size()) };

    // remove all prospects that do not match data
    for (size_t ticket_data_index{}; ticket_data_index < samples.size(); ++ticket_data_index) {
        auto const & ticket_data{ samples[ticket_data_index] };
        for (size_t field_index{}; field_index < field_rules.size(); ++field_index) {
            auto const & field{ field_rules[field_index] };
            if (any_of(ticket_data, [&field](number_t const number) { return !field.contains(number); })) {
                auto & prospect{ prospect_list[ticket_data_index] };
                auto const search_result{ lower_bound(prospect, field_index) };
                prospect.erase(search_result);
            }
        }
    }

    // use deduction to single-out indexes
    auto const to_remove_from_begin{ partition(prospect_list,
                                               [](Prospects const & prospects) { return prospects.size() == 1; }) };
    deduce(prospect_list.cbegin(), to_remove_from_begin, to_remove_from_begin, prospect_list.end());

    // filter departure fields
    number_t result{ 1 };
    for (size_t ticket_data_index{}; ticket_data_index < prospect_list.size(); ++ticket_data_index) {
        assert(prospect_list[ticket_data_index].size() == 1);
        auto const rule_index{ prospect_list[ticket_data_index].front() };
        auto const & rule{ field_rules[rule_index] };
        if (rule.is_departure()) {
            auto const value_on_my_ticket{ my_ticket[ticket_data_index] };
            result *= value_on_my_ticket;
        }
    }

    return result;
}

//==============================================================================
struct Day_15_Data {
    std::vector<Field_Rule> field_rules;
    std::vector<number_t> my_ticket_values;
    std::vector<std::vector<number_t>> nearby_tickets_values;
    //==============================================================================
    static Day_15_Data from_string(std::string_view const & string)
    {
        std::string_view ticket_fields_string;
        std::string_view my_ticket_values_string;
        std::string_view nearby_tickets_values_string;
        scan(string,
             "{}\n\nyour ticket:\n{}\n\nnearby tickets:\n{}",
             ticket_fields_string,
             my_ticket_values_string,
             nearby_tickets_values_string);

        return Day_15_Data{ Field_Rule::from_list_string(ticket_fields_string),
                            scan_list<number_t>(my_ticket_values_string, ','),
                            parse_ticket_values(nearby_tickets_values_string) };
    }
};

//==============================================================================
std::string day_16_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const info{ Day_15_Data::from_string(input) };
    auto const error_rate{ get_ticket_scanning_error_rate(info.nearby_tickets_values, info.field_rules) };

    return std::to_string(error_rate);
}

//==============================================================================
std::string day_16_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const info{ Day_15_Data::from_string(input) };
    auto const departure_product{
        get_departure_product(info.nearby_tickets_values, info.field_rules, info.my_ticket_values)
    };

    return std::to_string(departure_product);
}