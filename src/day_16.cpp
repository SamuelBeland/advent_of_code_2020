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
struct Ticket_Field {
    std::string name;
    Range low_range;
    Range high_range;

    //==============================================================================
    [[nodiscard]] bool contains(number_t const point) const
    {
        return low_range.contains(point) || high_range.contains(point);
    }
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
    [[nodiscard]] number_t get_ticket_scanning_error_rate() const
    {
        auto const invalid_numbers{ get_invalid_numbers() };

        auto const result{ std::accumulate(invalid_numbers.cbegin(), invalid_numbers.cend(), number_t{}) };
        return result;
    }
    [[nodiscard]] number_t get_departure_product() const
    {
        auto const valid_tickets{ get_valid_tickets() };
        std::vector<size_t> ticket_fields_indexes{};
        ticket_fields_indexes.reserve(ticket_fields.size());

        std::vector<size_t> prospects{};
        auto const remove_incompatible_prospects = [this, &prospects](number_t const number) {
            auto const valid_prospects_end{ std::remove_if(
                prospects.begin(),
                prospects.end(),
                [this, number](size_t const field_index) { return !ticket_fields[field_index].contains(number); }) };
            prospects.erase(valid_prospects_end, prospects.end());
        };

        for (size_t current_field_index{}; current_field_index < ticket_fields.size(); ++current_field_index) {
            prospects.resize(ticket_fields.size());
            std::iota(prospects.begin(), prospects.end(), size_t{});
        }
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

private:
    //==============================================================================
    [[nodiscard]] std::vector<number_t> get_invalid_numbers() const
    {
        auto const valid_ranges{ get_valid_ranges() };
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

        std::for_each(nearby_tickets_values.cbegin(), nearby_tickets_values.cend(), add_invalid_numbers);

        return invalid_numbers;
    }
    //==============================================================================
    [[nodiscard]] std::vector<Ticket_Values> get_valid_tickets() const
    {
        auto const valid_ranges{ get_valid_ranges() };
        auto result{ nearby_tickets_values };
        auto const valid_end{ std::remove_if(
            result.begin(),
            result.end(),
            [&valid_ranges](Ticket_Values const & values) {
                return std::all_of(
                    values.numbers.cbegin(),
                    values.numbers.cend(),
                    [&valid_ranges](number_t const number) {
                        return std::any_of(valid_ranges.cbegin(), valid_ranges.cend(), [number](Range const & range) {
                            return range.contains(number);
                        });
                    });
            }) };
        result.erase(valid_end, nearby_tickets_values.end());
        return result;
    }
    //==============================================================================
    [[nodiscard]] std::vector<Range> get_valid_ranges() const
    {
        std::vector<Range> valid_ranges{};
        std::function<void(Range const &)> const add_or_merge{ [&valid_ranges, &add_or_merge](Range const & new_range) {
            auto const colliding_range{ std::find_if(
                valid_ranges.begin(),
                valid_ranges.end(),
                [new_range](Range const & range) { return range.contains(new_range); }) };

            if (colliding_range != valid_ranges.end()) {
                auto const merged_new_range{ colliding_range->merge(new_range) };
                valid_ranges.erase(colliding_range);
                add_or_merge(merged_new_range);
            } else {
                valid_ranges.push_back(new_range);
            }
        } };

        for (auto const & field : ticket_fields) {
            add_or_merge(field.low_range);
            add_or_merge(field.high_range);
        }

        return valid_ranges;
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
    auto const input{ read_file(input_file_path) };
    auto info{ Info::from_string(input) };
    auto const departure_product{ info.get_departure_product() };

    return std::to_string(departure_product);
}