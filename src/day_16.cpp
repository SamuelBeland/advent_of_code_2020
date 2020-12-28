#include <algorithm>
#include <numeric>

#include "utils.hpp"
#include <resources.hpp>

using number_t = int;

//==============================================================================
struct Range {
    number_t min;
    number_t max;

    //==============================================================================
    bool contains(number_t const point) const { return point >= min && point <= max; }
    //==============================================================================
    bool contains(Range const & other) const { return contains(other.min) || contains(other.max); }
    //==============================================================================
    Range merge(Range const & other) const
    {
        return Range{ min < other.min ? min : other.min, max > other.max ? max : other.max };
    }
};

//==============================================================================
void add_or_merge(std::vector<Range> & valid_ranges, Range const & new_range)
{
    auto const colliding_range{ std::find_if(valid_ranges.begin(),
                                             valid_ranges.end(),
                                             [new_range](Range const & range) { return range.contains(new_range); }) };

    if (colliding_range != valid_ranges.end()) {
        auto const merged_new_range{ colliding_range->merge(new_range) };
        valid_ranges.erase(colliding_range);
        add_or_merge(valid_ranges, merged_new_range);
    } else {
        valid_ranges.push_back(new_range);
    }
};

//==============================================================================
struct Ticket_Field {
    std::string name;
    Range low_range;
    Range high_range;

    //==============================================================================
    static Ticket_Field from_string(std::string_view const & line)
    {
        Ticket_Field result{};
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
    static std::vector<Ticket_Field> from_list_string(std::string_view const & string)
    {
        auto const lines{ split(string) };
        std::vector<Ticket_Field> result{};
        result.reserve(lines.size());
        for (auto const & line : lines) {
            result.emplace_back(from_string(line));
        }
        return result;
    }
};

//==============================================================================
struct Ticket_Values {
    std::vector<number_t> numbers;

    //==============================================================================
    static Ticket_Values from_string(std::string_view const & line)
    {
        return Ticket_Values{ scan_list<number_t>(line, ',') };
    }
    //==============================================================================
    static std::vector<Ticket_Values> from_list_string(std::string_view const & string)
    {
        auto const lines{ split(string) };
        std::vector<Ticket_Values> result{};
        result.reserve(lines.size());
        for (auto const & line : lines) {
            result.emplace_back(from_string(line));
        }
        return result;
    }
};

//==============================================================================
struct Info {
    std::vector<Ticket_Field> ticket_fields;
    Ticket_Values my_ticket_values;
    std::vector<Ticket_Values> nearby_tickets_values;

    //==============================================================================
    number_t get_ticket_scanning_error_rate() const
    {
        std::vector<Range> valid_ranges{};

        for (auto const & field : ticket_fields) {
            add_or_merge(valid_ranges, field.low_range);
            add_or_merge(valid_ranges, field.high_range);
        }

        std::sort(valid_ranges.begin(), valid_ranges.end(), [](Range const & a, Range const & b) {
            return a.min < b.min;
        });

        std::vector<number_t> invalid_numbers{};
        auto const add_invalid_numbers = [&invalid_numbers, &valid_ranges](Ticket_Values const & values) {
            for (auto const number : values.numbers) {
                auto const is_valid{ std::any_of(
                    valid_ranges.cbegin(),
                    valid_ranges.cend(),
                    [number](Range const & valid_range) { return valid_range.contains(number); }) };
                if (!is_valid) {
                    invalid_numbers.emplace_back(number);
                }
            }
        };

        add_invalid_numbers(my_ticket_values);
        for (auto const & ticket_values : nearby_tickets_values) {
            add_invalid_numbers(ticket_values);
        }

        auto const result{ std::accumulate(invalid_numbers.cbegin(), invalid_numbers.cend(), number_t{}) };
        return result;
    }
    //==============================================================================
    static Info from_string(std::string_view const & string)
    {
        std::string_view ticket_fields_string;
        std::string_view my_ticket_values_string;
        std::string_view nearby_tickets_values_string;
        scan(string,
             "{}\n\nyour ticket:\n{}\n\nnearby tickets:\n{}",
             ticket_fields_string,
             my_ticket_values_string,
             nearby_tickets_values_string);

        return Info{ Ticket_Field::from_list_string(ticket_fields_string),
                     Ticket_Values::from_string(my_ticket_values_string),
                     Ticket_Values::from_list_string(nearby_tickets_values_string) };
    }
};

//==============================================================================
std::string day_16_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const info{ Info::from_string(input) };
    auto const error_rate{ info.get_ticket_scanning_error_rate() };

    return std::to_string(error_rate);
}

//==============================================================================
std::string day_16_b(char const * input_file_path)
{
    return "not implemented";
}