#include <limits>
#include <numeric>
#include <unordered_map>

#include "utils.hpp"
#include <resources.hpp>

//==============================================================================
class Memory : public std::unordered_map<uint64_t, uint64_t>
{
    using unordered_map::unordered_map;

public:
    uint64_t sum_values() const
    {
        auto const sum{ std::accumulate(
            cbegin(),
            cend(),
            uint64_t{},
            [](uint64_t const value, Memory::value_type const & p) { return value + p.second; }) };
        return sum;
    }
};

//==============================================================================
struct Operation {
    uint64_t address;
    uint64_t value;
};

//==============================================================================
struct Mask {
    [[maybe_unused]] static auto constexpr LENGTH = 36;
    uint64_t zeros;
    uint64_t ones;

    uint64_t apply(uint64_t const & value) const
    {
        auto const zero_removed{ value & ~zeros };
        auto const result{ zero_removed | ones };
        return result;
    }
    template<typename Func>
    void for_all_permutations(uint64_t const address, Func const & func) const
    {
        for_all_permutations_(func, ones | address, 0);
    }

private:
    template<typename Func>
    void for_all_permutations_(Func const & func, uint64_t const value, size_t const shift) const
    {
        if (shift == LENGTH) {
            func(value);
            return;
        }

        auto const mask{ uint64_t{ 1 } << shift };
        if (mask & (ones | zeros)) {
            // value is set : keep on going
            for_all_permutations_(func, value, shift + 1);
        } else {
            // value is floating, branch
            for_all_permutations_(func, value, shift + 1);
            for_all_permutations_(func, value ^ mask, shift + 1);
        }
    }
};

//==============================================================================
struct Init_Section {
    Mask mask;
    std::vector<Operation> operations;
};

//==============================================================================
Mask parse_mask(std::string_view const & string)
{
    assert(string.size() == Mask::LENGTH);
    Mask result{ 0, 0 };

    for (auto it{ string.cbegin() }; it != string.cend(); ++it) {
        result.ones <<= 1;
        result.zeros <<= 1;
        if (*it == '1') {
            ++result.ones;
        } else if (*it == '0') {
            ++result.zeros;
        } else {
            assert(*it == 'X');
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

#include <iostream>

//==============================================================================
std::string day_14_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const init_sequence{ parse_init_sequence(input) };

    Memory memory{};
    for (auto const & section : init_sequence) {
        for (auto const & operation : section.operations) {
            auto const masked_value{ section.mask.apply(operation.value) };
            memory[operation.address] = masked_value;
        }
    }

    auto const sum{ memory.sum_values() };

    return std::to_string(sum);
}

//==============================================================================
std::string day_14_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const init_sequence{ parse_init_sequence(input) };

    Memory memory{};

    for (auto const & section : init_sequence) {
        for (auto const & operation : section.operations) {
            section.mask.for_all_permutations(operation.address, [&memory, &operation](uint64_t const address) {
                memory[address] = operation.value;
            });
        }
    }

    auto const sum{ memory.sum_values() };

    return std::to_string(sum);
}