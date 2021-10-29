//-- - Day 3: Toboggan Trajectory-- -
// With the toboggan login problems resolved, you set off toward the airport.While travel by toboggan might be easy,
// it's certainly not safe: there's very minimal steering and the area is covered in trees.You'll need to see which
// angles will take you near the fewest trees.
//
// Due to the local geology, trees in this area only grow on exact integer coordinates in a grid.You make a map(your
// puzzle input) of the open squares(.) and trees(#) you can see.For example :
//
// ..##.......
// #...#...#..
// .#....#..#.
// ..#.#...#.#
// .#...##..#.
// ..#.##.....
// .#.#.#....#
// .#........#
// #.##...#...
// #...##....#
// .#..#...#.#
//
// These aren't the only trees, though; due to something you read about once involving arboreal genetics and biome
// stability, the same pattern repeats to the right many times:
//
// ..##.........##.........##.........##.........##.........##.......  --->
// #...#...#..#...#...#..#...#...#..#...#...#..#...#...#..#...#...#..
// .#....#..#..#....#..#..#....#..#..#....#..#..#....#..#..#....#..#.
// ..#.#...#.#..#.#...#.#..#.#...#.#..#.#...#.#..#.#...#.#..#.#...#.#
// .#...##..#..#...##..#..#...##..#..#...##..#..#...##..#..#...##..#.
// ..#.##.......#.##.......#.##.......#.##.......#.##.......#.##.....  --->
// .#.#.#....#.#.#.#....#.#.#.#....#.#.#.#....#.#.#.#....#.#.#.#....#
// .#........#.#........#.#........#.#........#.#........#.#........#
// #.##...#...#.##...#...#.##...#...#.##...#...#.##...#...#.##...#...
// #...##....##...##....##...##....##...##....##...##....##...##....#
// .#..#...#.#.#..#...#.#.#..#...#.#.#..#...#.#.#..#...#.#.#..#...#.#  --->
//
// You start on the open square(.) in the top - left corner and need to reach the bottom(below the bottom - most row on
// your map).
//
// The toboggan can only follow a few specific slopes(you opted for a cheaper model that prefers rational numbers);
// start by counting all the trees you would encounter for the slope right 3, down 1:
//
// From your starting position at the top - left, check the position that is right 3 and down 1. Then, check the
// position that is right 3 and down 1 from there, and so on until you go past the bottom of the map.
//
// The locations you'd check in the above example are marked here with O where there was an open square and X where
// there was a tree:
//
// ..##.........##.........##.........##.........##.........##.......  --->
// #..O#...#..#...#...#..#...#...#..#...#...#..#...#...#..#...#...#..
// .#....X..#..#....#..#..#....#..#..#....#..#..#....#..#..#....#..#.
// ..#.#...#O#..#.#...#.#..#.#...#.#..#.#...#.#..#.#...#.#..#.#...#.#
// .#...##..#..X...##..#..#...##..#..#...##..#..#...##..#..#...##..#.
// ..#.##.......#.X#.......#.##.......#.##.......#.##.......#.##.....  --->
// .#.#.#....#.#.#.#.O..#.#.#.#....#.#.#.#....#.#.#.#....#.#.#.#....#
// .#........#.#........X.#........#.#........#.#........#.#........#
// #.##...#...#.##...#...#.X#...#...#.##...#...#.##...#...#.##...#...
// #...##....##...##....##...#X....##...##....##...##....##...##....#
// .#..#...#.#.#..#...#.#.#..#...X.#.#..#...#.#.#..#...#.#.#..#...#.#  --->
//
// In this example, traversing the map using this slope would cause you to encounter 7 trees.
//
// Starting at the top - left corner of your map and following a slope of right 3 and down 1, how many trees would you
// encounter ?
//
// Your puzzle answer was 265.
//
// The first half of this puzzle is complete!It provides one gold star : *
//
//-- - Part Two-- -
// Time to check the rest of the slopes - you need to minimize the probability of a sudden arboreal stop, after all.
//
// Determine the number of trees you would encounter if, for each of the following slopes, you start at the top - left
// corner and traverse the map all the way to the bottom :
//
// Right 1, down 1.
// Right 3, down 1. (This is the slope you already checked.)
// Right 5, down 1.
// Right 7, down 1.
// Right 1, down 2.
//
// In the above example, these slopes would find 2, 7, 3, 4, and 2 tree(s) respectively; multiplied together, these
// produce the answer 336.
//
// What do you get if you multiply together the number of trees encountered on each of the listed slopes ?

#include "utils.hpp"
#include <resources.hpp>

namespace
{
//==============================================================================
enum class Position { free, tree };

//==============================================================================
struct Slope {
    size_t x_diff;
    size_t y_diff;
};

//==============================================================================
Position parse_position(char const character)
{
    assert(character == '.' || character == '#');
    return character == '.' ? Position::free : Position::tree;
}

//==============================================================================
class Forest
{
    size_t m_width;
    size_t m_height;
    std::vector<Position> m_data;

public:
    //==============================================================================
    explicit Forest(char const * input_file_path)
    {
        auto const input{ aoc::read_file(input_file_path) };
        auto const lines{ aoc::split(input) };

        m_height = lines.size();
        m_width = lines.front().size();
        assert(std::all_of(lines.cbegin() + 1, lines.cend(), [this](std::string_view const & line) {
            return line.size() == m_width;
        }));

        m_data.reserve(m_width * m_height);

        for (auto const & line : lines) {
            for (auto const & character : line) {
                m_data.emplace_back(parse_position(character));
            }
        }
    }

    //==============================================================================
    [[nodiscard]] size_t count_trees_in_slope(Slope const & slope) const
    {
        size_t x{};
        size_t y{};
        size_t count{};

        while (y < m_height) {
            if (get_position(x, y) == Position::tree) {
                ++count;
            }
            x += slope.x_diff;
            y += slope.y_diff;
            if (x >= m_width) {
                x -= m_width;
            }
        }

        return count;
    }

private:
    //==============================================================================
    [[nodiscard]] Position get_position(size_t const x, size_t const y) const
    {
        assert(x < m_width);
        assert(y < m_height);
        return m_data[x + y * m_width];
    }
};

} // namespace

//==============================================================================
std::string day_3_a(char const * input_file_path)
{
    static constexpr Slope SLOPE{ 3, 1 };

    Forest const forest{ input_file_path };

    auto const tree_count{ forest.count_trees_in_slope(SLOPE) };
    return std::to_string(tree_count);
}

//==============================================================================
std::string day_3_b(char const * input_file_path)
{
    static constexpr std::array<Slope, 5> SLOPES{ Slope{ 1, 1 },
                                                  Slope{ 3, 1 },
                                                  Slope{ 5, 1 },
                                                  Slope{ 7, 1 },
                                                  Slope{ 1, 2 } };

    Forest const forest{ input_file_path };

    auto const count_trees_in_slope = [&forest](Slope const & slope) { return forest.count_trees_in_slope(slope); };

    auto const product_of_tree_counts{
        aoc::transform_reduce(SLOPES, size_t{ 1 }, count_trees_in_slope, std::multiplies())
    };

    return std::to_string(product_of_tree_counts);
}