#include "utils.hpp"
#include <resources.hpp>

#include <array>
#include <cassert>
#include <charconv>

enum class Operation { acc, jmp, nop };

using argument_t = int;

struct Instruction {
    Operation operation;
    argument_t argument;
};

using Memory = std::vector<Instruction>;

//==============================================================================
Operation parse_operation(std::string_view const & string)
{
    assert(string.size() == 3);
    switch (string[0]) {
    case 'n':
        assert(string == "nop");
        return Operation::nop;
    case 'a':
        assert(string == "acc");
        return Operation::acc;
    case 'j':
        assert(string == "jmp");
        return Operation::jmp;
    }
    assert(false);
    return {};
}

//==============================================================================
argument_t parse_argument(std::string_view const & string)
{
    char const * begin{ string.data() };
    char const * const end{ string.data() + string.size() };
    if (string[0] == '+') {
        ++begin;
    } else {
        assert(*begin == '-');
    }
    argument_t argument;
    [[maybe_unused]] auto const from_char_result{ std::from_chars(begin, end, argument) };
    assert(from_char_result.ec == std::errc());
    return argument;
}

//==============================================================================
Instruction parse_instruction(std::string_view const & line)
{
    std::string_view operation_string;
    std::string_view argument_string;
    scan(line, "{} {}", operation_string, argument_string);
    Instruction const result{ parse_operation(operation_string), parse_argument(argument_string) };
    return result;
}

//==============================================================================
Memory parse_memory(std::string_view const & input)
{
    auto const lines{ split(input) };
    Memory memory{};
    memory.resize(lines.size());
    std::transform(lines.cbegin(), lines.cend(), memory.begin(), parse_instruction);
    return memory;
}

//==============================================================================
class Console
{
    Memory m_memory{};
    argument_t m_accumulator{};
    size_t m_stack_pointer{};

public:
    //==============================================================================
    Console(Memory memory) : m_memory(std::move(memory)) {}
    //==============================================================================
    void execute_instruction(Instruction const & instruction)
    {
        switch (instruction.operation) {
        case Operation::acc:
            m_accumulator += instruction.argument;
            ++m_stack_pointer;
            break;
        case Operation::jmp:
            m_stack_pointer += instruction.argument;
            break;
        case Operation::nop:
            ++m_stack_pointer;
            break;
        default:
            assert(false);
        }
    }
    //==============================================================================
    void run_until_infinite_loop()
    {
        std::vector<bool> visited_addresses{};
        visited_addresses.resize(m_memory.size());

        while (!visited_addresses[m_stack_pointer]) {
            visited_addresses[m_stack_pointer] = true;
            execute_instruction(m_memory[m_stack_pointer]);
        }
    }
    //==============================================================================
    argument_t get_accumulator_value() const { return m_accumulator; }
};

//==============================================================================
std::string day_8_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    Console console{ parse_memory(input) };
    console.run_until_infinite_loop();
    auto const accumulator_value{ console.get_accumulator_value() };
    return std::to_string(accumulator_value);
}

//==============================================================================
std::string day_8_b(char const * input_file_path)
{
    return "day_8_b not implemented!";
}
