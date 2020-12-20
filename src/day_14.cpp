#include <limits>
#include <numeric>
#include <unordered_map>

#include "utils.hpp"
#include <resources.hpp>

//==============================================================================
using Memory = std::unordered_map<uint64_t, uint64_t>;

//==============================================================================
struct Operation {
    uint64_t address;
    uint64_t value;
};

//==============================================================================
struct Mask {
    uint64_t force_zeros;
    uint64_t force_ones;
};

//==============================================================================
struct Init_Section {
    Mask mask;
    std::vector<Operation> operations;
};

//==============================================================================
Mask parse_mask(std::string_view const & string)
{
    [[maybe_unused]] static auto constexpr MASK_LENGTH = 36;

    assert(string.size() == MASK_LENGTH);
    Mask result{ 0, std::numeric_limits<uint64_t>::max() };

    for (auto it{ string.cbegin() }; it != string.cend(); ++it) {
        result.force_ones <<= 1;
        result.force_zeros <<= 1;
        if (*it == '1') {
            ++result.force_ones;
            ++result.force_zeros;
        } else if (*it == 'X') {
            ++result.force_zeros;
            ;
        } else {
            assert(*it == '0');
        }
    }

    return result;
}

//==============================================================================
std::vector<Init_Section> parse_init_sequence(std::string const & input)
{
    auto const lines{ split(input) };
    std::vector<Init_Section> result;

    for (auto const & line : lines) {
        if (line[1] == 'a') {
            // first word is "mask" : mask definition line
            std::string_view mask;
            scan(line, "mask = {}", mask);
            Init_Section new_init_section;
            new_init_section.mask = parse_mask(mask);
            result.push_back(new_init_section);
        } else {
            // first word is "mem" : memory assignation line
            assert(line[1] == 'e');
            uint64_t address;
            uint64_t value;
            scan(line, "mem[{}] = {}", address, value);
            result.back().operations.push_back(Operation{ address, value });
        }
    }

    return result;
}

//==============================================================================
std::string day_14_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const init_sequence{ parse_init_sequence(input) };

    Memory memory{};
    for (auto const & section : init_sequence) {
        for (auto const & operation : section.operations) {
            auto const forced_ones{ operation.value | section.mask.force_ones };
            auto const forced_zeros{ forced_ones & section.mask.force_zeros };
            memory[operation.address] = forced_zeros;
        }
    }

    auto const sum{ std::accumulate(
        memory.cbegin(),
        memory.cend(),
        uint64_t{},
        [](uint64_t const value, Memory::value_type const & p) { return value + p.second; }) };

    return std::to_string(sum);
}

//==============================================================================
std::string day_14_b(char const * input_file_path)
{
    return std::to_string(0);
}