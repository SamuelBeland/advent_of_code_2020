//-- - Day 8: Handheld Halting-- -
// Your flight to the major airline hub reaches cruising altitude without incident.While you consider checking the in -
// flight menu for one of those drinks that come with a little umbrella, you are interrupted by the kid sitting next to
// you.
//
// Their handheld game console won't turn on! They ask if you can take a look.
//
// You narrow the problem down to a strange infinite loop in the boot code(your puzzle input) of the device.You should
// be able to fix it, but first you need to be able to run the code in isolation.
//
// The boot code is represented as a text file with one instruction per line of text.Each instruction consists of an
// operation(acc, jmp, or nop) and an argument(a signed number like + 4 or -20).
//
// acc increases or decreases a single global value called the accumulator by the value given in the argument.For
// example, acc + 7 would increase the accumulator by 7. The accumulator starts at 0. After an acc instruction, the
// instruction immediately below it is executed next. jmp jumps to a new instruction relative to itself.The next
// instruction to execute is found using the argument as an offset from the jmp instruction; for example, jmp + 2 would
// skip the next instruction, jmp + 1 would continue to the instruction immediately below it, and jmp - 20 would cause
// the instruction 20 lines above to be executed next. nop stands for No OPeration - it does nothing.The instruction
// immediately below it is executed next.
//
// For example, consider the following program :
//
// nop + 0
// acc + 1
// jmp + 4
// acc + 3
// jmp - 3
// acc - 99
// acc + 1
// jmp - 4
// acc + 6
//
// These instructions are visited in this order:
//
// nop + 0 | 1
// acc + 1 | 2, 8(!)
// jmp + 4 | 3
// acc + 3 | 6
// jmp - 3 | 7
// acc - 99 |
// acc + 1 | 4
// jmp - 4 | 5
// acc + 6 |
//
// First, the nop + 0 does nothing.Then, the accumulator is increased from 0 to 1 (acc + 1) and jmp + 4 sets the next
// instruction to the other acc + 1 near the bottom.After it increases the accumulator from 1 to 2, jmp - 4 executes,
// setting the next instruction to the only acc + 3. It sets the accumulator to 5, and jmp - 3 causes the program to
// continue back at the first acc + 1.
//
// This is an infinite loop : with this sequence of jumps, the program will run forever.The moment the program tries to
// run any instruction a second time, you know it will never terminate.
//
// Immediately before the program would run an instruction a second time, the value in the accumulator is 5.
//
// Run your copy of the boot code.Immediately before any instruction is executed a second time, what value is in the
// accumulator ?
//
//-- - Part Two-- -
// After some careful analysis, you believe that exactly one instruction is corrupted.
//
// Somewhere in the program, either a jmp is supposed to be a nop, or a nop is supposed to be a jmp. (No acc
// instructions were harmed in the corruption of this boot code.)
//
// The program is supposed to terminate by attempting to execute an instruction immediately after the last instruction
// in the file.By changing exactly one jmp or nop, you can repair the boot codeand make it terminate correctly.
//
// For example, consider the same program from above :
//
// nop + 0
// acc + 1
// jmp + 4
// acc + 3
// jmp - 3
// acc - 99
// acc + 1
// jmp - 4
// acc + 6
//
// If you change the first instruction from nop + 0 to jmp + 0, it would create a single - instruction infinite loop,
// never leaving that instruction.If you change almost any of the jmp instructions, the program will still eventually
// find another jmp instructionand loop forever.
//
// However, if you change the second - to - last instruction(from jmp - 4 to nop - 4), the program terminates!The
// instructions are visited in this order:
//
// nop + 0 | 1
// acc + 1 | 2
// jmp + 4 | 3
// acc + 3 |
// jmp - 3 |
// acc - 99 |
// acc + 1 | 4
// nop - 4 | 5
// acc + 6 | 6
//
// After the last instruction(acc + 6), the program terminates by attempting to run the instruction below the last
// instruction in the file.With this change, after the program terminates, the accumulator contains the value 8 (acc +
// 1, acc + 1, acc + 6).
//
// Fix the program so that it terminates normally by changing exactly one jmp(to nop) or nop(to jmp).What is the value
// of the accumulator after the program terminates ?

#include "utils.hpp"
#include <resources.hpp>

namespace
{
//==============================================================================
enum class Operation { acc, jmp, nop };

//==============================================================================
using argument_t = int;

//==============================================================================
constexpr Operation parse_operation(aoc::StringView const & string)
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
argument_t parse_argument(aoc::StringView const & string)
{
    assert(!string.empty());
    assert(string.front() == '-' || string.front() == '+');

    auto const startsWithPlus{ string.front() == '+' };

    auto const clean_string{ startsWithPlus ? string.remove_from_start(1) : string };
    return clean_string.parse<argument_t>();
}

//==============================================================================
struct Instruction {
    Operation operation;
    argument_t argument;
    //==============================================================================
    static Instruction from_string(aoc::StringView const & line)
    {
        aoc::StringView operation_string;
        aoc::StringView argument_string;
        line.scan("{} {}", operation_string, argument_string);
        auto const operation{ parse_operation(operation_string) };
        auto const argument{ parse_argument(argument_string) };
        Instruction const result{ operation, argument };
        return result;
    }
};

//==============================================================================
using Memory = std::vector<Instruction>;

//==============================================================================
Memory parse_memory(aoc::StringView const & input)
{
    return input.iterate_transform(Instruction::from_string, '\n');
}

//==============================================================================
class Console
{
    Memory m_memory{};
    argument_t m_accumulator{};
    size_t m_stack_pointer{};

public:
    //==============================================================================
    enum class Debug_Code { no_error, infinite_loop };
    //==============================================================================
    explicit Console(Memory memory) noexcept : m_memory(std::move(memory)) {}
    //==============================================================================
    Debug_Code debug()
    {
        std::vector<bool> visited_addresses{};
        visited_addresses.resize(m_memory.size());

        while (m_stack_pointer < m_memory.size() && !visited_addresses[m_stack_pointer]) {
            visited_addresses[m_stack_pointer] = true;
            execute_instruction(m_memory[m_stack_pointer]);
        }
        assert(m_stack_pointer <= m_memory.size());
        return m_stack_pointer == m_memory.size() ? Debug_Code::no_error : Debug_Code::infinite_loop;
    }
    //==============================================================================
    void fix_corrupted_instruction() noexcept(!aoc::detail::IS_DEBUG)
    {
        auto const swap_operations = [this](size_t const & address) {
            auto & operation{ m_memory[address].operation };
            assert(operation == Operation::jmp || operation == Operation::nop);
            operation = (operation == Operation::jmp ? Operation::nop : Operation::jmp);
        };

        auto const get_next_jmp_or_nop_address = [this](size_t stack_pointer) {
            while (m_memory[stack_pointer].operation != Operation::jmp
                   && m_memory[stack_pointer].operation != Operation::nop) {
                ++stack_pointer;
            }
            return stack_pointer;
        };

        auto debug_cursor{ get_next_jmp_or_nop_address(0) };
        swap_operations(debug_cursor);
        auto debug_code{ debug() };
        while (debug_code != Debug_Code::no_error) {
            swap_operations(debug_cursor);
            debug_cursor = get_next_jmp_or_nop_address(debug_cursor + 1);
            swap_operations(debug_cursor);
            m_stack_pointer = 0;
            m_accumulator = 0;
            debug_code = debug();
        }
    }
    //==============================================================================
    [[nodiscard]] argument_t get_accumulator_value() const { return m_accumulator; }

private:
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
};

} // namespace

//==============================================================================
std::string day_8_a(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    Console console{ parse_memory(input) };
    console.debug();
    auto const accumulator_value{ console.get_accumulator_value() };
    return std::to_string(accumulator_value);
}

//==============================================================================
std::string day_8_b(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    Console console{ parse_memory(input) };
    console.fix_corrupted_instruction();
    auto const result{ console.get_accumulator_value() };
    return std::to_string(result);
}
