#include <numeric>

#include "utils.hpp"
#include <resources.hpp>

enum class Action : char { north = 'N', south = 'S', east = 'E', west = 'W', left = 'L', right = 'R', forward = 'F' };

struct Step {
    Action action;
    int amount;
};

Step parse_step(std::string_view const & line)
{
    assert(line.size() >= 2);
    auto const action{ static_cast<Action>(line.front()) };
    int amount;
    [[maybe_unused]] auto const from_chars_result{
        std::from_chars(line.data() + 1, line.data() + line.size(), amount)
    };
    assert(from_chars_result.ec == std::errc());
    Step const step{ action, amount };
    return step;
}

enum class Direction { north, east, south, west, MAX };

struct Position {
    int x;
    int y;
    Direction direction;

    void advance(int const amount) { advance(amount, direction); }
    void advance(int const amount, Direction const direction_)
    {
        switch (direction_) {
        case Direction::north:
            y -= amount;
            return;
        case Direction::east:
            x += amount;
            return;
        case Direction::south:
            y += amount;
            return;
        case Direction::west:
            x -= amount;
            return;
        }
        assert(false);
    }
};

Direction rotate(Direction const initial_direction, int const amount, bool const clockwise)
{
    static constexpr auto MAX_DIRECTION{ static_cast<int>(Direction::MAX) };

    auto new_angle{ clockwise ? static_cast<int>(initial_direction) + amount / 90
                              : static_cast<int>(initial_direction) - amount / 90 };
    if (new_angle < 0) {
        new_angle += MAX_DIRECTION;
    } else if (new_angle >= MAX_DIRECTION) {
        new_angle -= MAX_DIRECTION;
    }
    return static_cast<Direction>(new_angle);
}

Position & apply_step(Position & position, Step const & step)
{
    switch (step.action) {
    case Action::forward:
        position.advance(step.amount);
        return position;
    case Action::north:
        position.advance(step.amount, Direction::north);
        return position;
    case Action::east:
        position.advance(step.amount, Direction::east);
        return position;
    case Action::south:
        position.advance(step.amount, Direction::south);
        return position;
    case Action::west:
        position.advance(step.amount, Direction::west);
        return position;
    case Action::right:
        position.direction = rotate(position.direction, step.amount, true);
        return position;
    case Action::left:
        position.direction = rotate(position.direction, step.amount, false);
        return position;
    }
}

//==============================================================================
std::string day_12_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    auto const position{
        std::transform_reduce(lines.cbegin(), lines.cend(), Position{ 0, 0, Direction::east }, apply_step, parse_step)
    };

    auto const manhattan_distance{ std::abs(position.x) + std::abs(position.y) };

    return std::to_string(manhattan_distance);
}

//==============================================================================
std::string day_12_b(char const * input_file_path)
{
    return "day_12_b not implemented";
}