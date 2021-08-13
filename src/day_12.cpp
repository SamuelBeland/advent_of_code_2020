//-- - Day 12: Rain Risk-- -
// Your ferry made decent progress toward the island, but the storm came in faster than anyone expected.The ferry needs
// to take evasive actions!
//
// Unfortunately, the ship's navigation computer seems to be malfunctioning; rather than giving a route directly to
// safety, it produced extremely circuitous instructions. When the captain uses the PA system to ask if anyone can help,
// you quickly volunteer.
//
// The navigation instructions(your puzzle input) consists of a sequence of single - character actions paired with
// integer input values.After staring at them for a few minutes, you work out what they probably mean :
//
// Action N means to move north by the given value.
// Action S means to move south by the given value.
// Action E means to move east by the given value.
// Action W means to move west by the given value.
// Action L means to turn left the given number of degrees.
// Action R means to turn right the given number of degrees.
// Action F means to move forward by the given value in the direction the ship is currently facing.
//
// The ship starts by facing east.Only the Land R actions change the direction the ship is facing. (That is, if the ship
// is facing eastand the next instruction is N10, the ship would move north 10 units, but would still move east if the
// following action were F.)
//
// For example :
//
// F10
// N3
// F7
// R90
// F11
//
// These instructions would be handled as follows :
//
// F10 would move the ship 10 units east(because the ship starts by facing east) to east 10, north 0.
// N3 would move the ship 3 units north to east 10, north 3.
// F7 would move the ship another 7 units east(because the ship is still facing east) to east 17, north 3.
// R90 would cause the ship to turn right by 90 degrees and face south; it remains at east 17, north 3.
// F11 would move the ship 11 units south to east 17, south 8.
//
// At the end of these instructions, the ship's Manhattan distance (sum of the absolute values of its east/west position
// and its north/south position) from its starting position is 17 + 8 = 25.
//
// Figure out where the navigation instructions lead.What is the Manhattan distance between that location and the ship's
// starting position?
//
// The first half of this puzzle is complete!It provides one gold star : *
//
//-- - Part Two-- -
// Before you can give the destination to the captain, you realize that the actual action meanings were printed on the
// back of the instructions the whole time.
//
// Almost all of the actions indicate how to move a waypoint which is relative to the ship's position:
//
// Action N means to move the waypoint north by the given value.
// Action S means to move the waypoint south by the given value.
// Action E means to move the waypoint east by the given value.
// Action W means to move the waypoint west by the given value.
// Action L means to rotate the waypoint around the ship left(counter - clockwise) the given number of degrees.
// Action R means to rotate the waypoint around the ship right(clockwise) the given number of degrees.
// Action F means to move forward to the waypoint a number of times equal to the given value.
//
// The waypoint starts 10 units east and 1 unit north relative to the ship. The waypoint is relative to the ship; that
// is, if the ship moves, the waypoint moves with it.
//
// For example, using the same instructions as above :
//
// F10 moves the ship to the waypoint 10 times(a total of 100 units east and 10 units north), leaving the ship at east
// 100, north 10. The waypoint stays 10 units east and 1 unit north of the ship. N3 moves the waypoint 3 units north to
// 10 units east and 4 units north of the ship.The ship remains at east 100, north 10. F7 moves the ship to the waypoint
// 7 times(a total of 70 units east and 28 units north), leaving the ship at east 170, north 38. The waypoint stays 10
// units east and 4 units north of the ship. R90 rotates the waypoint around the ship clockwise 90 degrees, moving it to
// 4 units east and 10 units south of the ship.The ship remains at east 170, north 38. F11 moves the ship to the
// waypoint 11 times(a total of 44 units east and 110 units south), leaving the ship at east 214, south 72. The waypoint
// stays 4 units east and 10 units south of the ship.
//
// After these operations, the ship's Manhattan distance from its starting position is 214 + 72 = 286.
//
// Figure out where the navigation instructions actually lead.What is the Manhattan distance between that location and
// the ship's starting position?

#include "utils.hpp"
#include <resources.hpp>

namespace
{
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

struct Point {
    int x;
    int y;

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
    Point & rotate_around_origin(int const angle, bool const clockwise)
    {
        static constexpr auto MAX_DIRECTION{ static_cast<int>(Direction::MAX) };

        assert(angle % 90 == 0 && angle >= 0 && angle <= 270);
        auto const number_of_rotations{ angle / 90 };

        for (int i{}; i < number_of_rotations; ++i) {
            if (clockwise) {
                y *= -1;
                std::swap(x, y);
            } else {
                std::swap(x, y);
                y *= -1;
            }
        }

        return *this;
    }

    Point operator*(int const amount) const
    {
        Point const result{ x * amount, y * amount };
        return result;
    }
    Point & operator+=(Point const & other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
};

struct Position : Point {
    Direction direction;

    void advance(int const amount) { Point::advance(amount, direction); }
    void advance(int const amount, Direction const direction_) { Point::advance(amount, direction_); }
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
    assert(false);
    return position;
}

} // namespace

//==============================================================================
std::string day_12_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    auto const position{ transform_reduce(lines, Position{ 0, 0, Direction::east }, parse_step, apply_step) };

    auto const manhattan_distance{ std::abs(position.x) + std::abs(position.y) };

    return std::to_string(manhattan_distance);
}

//==============================================================================
std::string day_12_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    std::vector<Step> steps{};
    steps.reserve(lines.size());

    for (auto const & line : lines) {
        steps.emplace_back(parse_step(line));
    }

    Point boat{ 0, 0 };
    Point waypoint{ 10, -1 };

    for (auto const & step : steps) {
        switch (step.action) {
        case Action::north:
            waypoint.advance(step.amount, Direction::north);
            break;
        case Action::east:
            waypoint.advance(step.amount, Direction::east);
            break;
        case Action::south:
            waypoint.advance(step.amount, Direction::south);
            break;
        case Action::west:
            waypoint.advance(step.amount, Direction::west);
            break;
        case Action::left:
            waypoint.rotate_around_origin(step.amount, false);
            break;
        case Action::right:
            waypoint.rotate_around_origin(step.amount, true);
            break;
        case Action::forward:
            boat += waypoint * step.amount;
            break;
        default:
            assert(false);
        }
    }

    auto const manhattan_distance{ std::abs(boat.x) + std::abs(boat.y) };
    return std::to_string(manhattan_distance);
}