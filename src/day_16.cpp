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
    [[nodiscard]] bool is_departure() const { return name.find("departure") == 0; }
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

struct Prospect
{
    size_t 
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
        auto const rotate = [](std::vector<Ticket_Values> const & values_to_rotate) {
            std::vector<std::vector<number_t>> data{};
            auto const number_of_fields{ values_to_rotate.front().numbers.size() };
            auto const number_of_tickets{ values_to_rotate.size() };
            data.resize(number_of_fields);
            for (size_t field_index{}; field_index < number_of_fields; ++field_index) {
                data[field_index].resize(number_of_tickets);
                std::transform(
                    values_to_rotate.cbegin(),
                    values_to_rotate.cend(),
                    data[field_index].begin(),
                    [field_index](Ticket_Values const & ticket_values) { return ticket_values.numbers[field_index]; });
            }
            return data;
        };

        auto const rotated_ticket_data{ rotate(get_valid_tickets()) };
        std::vector<std::vector<size_t>> ticket_fields_indexes{};
        ticket_fields_indexes.reserve(ticket_fields.size());

        std::vector<std::vector<size_t>> incompatible_field_indexes{ rotated_ticket_data.size() };

        for (size_t ticket_data_index{}; ticket_data_index < rotated_ticket_data.size(); ++ticket_data_index) {
            auto const & ticket_data{ rotated_ticket_data[ticket_data_index] };
            for (size_t field_index{}; field_index < ticket_fields.size(); ++field_index) {
                auto const & field{ ticket_fields[field_index] };
                auto const is_invalid{ std::any_of(
                    ticket_data.cbegin(),
                    ticket_data.cend(),
                    [&field](number_t const number) { return !field.contains(number); }) };
                if (is_invalid) {
                    incompatible_field_indexes[ticket_data_index].push_back(field_index);
                }
            }
        }

        for (size_t field_index{}; field_index < )

        return 0;
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
        auto candidate_tickets{ nearby_tickets_values };
        auto const candidate_tickets_valid_end{ std::remove_if(
            candidate_tickets.begin(),
            candidate_tickets.end(),
            [&valid_ranges](Ticket_Values const & values) {
                return std::any_of(
                    values.numbers.cbegin(),
                    values.numbers.cend(),
                    [&valid_ranges](number_t const number) {
                        return std::all_of(valid_ranges.cbegin(), valid_ranges.cend(), [number](Range const & range) {
                            return !range.contains(number);
                        });
                    });
            }) };
        candidate_tickets.erase(candidate_tickets_valid_end, candidate_tickets.end());
        return candidate_tickets;
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