#include "utils.hpp"
#include <resources.hpp>

class Ferry;

//==============================================================================
class Ferry
{
public:
    enum class Tile { floor, empty, occupied };

private:
    //==============================================================================
    size_t m_width;
    size_t m_height;
    std::vector<Tile> m_tiles;

    using Counting_Function = size_t (Ferry::*)(size_t) const;

public:
    //==============================================================================
    Ferry(std::string const & input)
    {
        auto const lines{ split(input) };

        m_width = lines.front().size();
        m_height = lines.size();

        m_tiles.reserve(m_width * m_height);

        for (auto const & line : lines) {
            assert(line.size() == m_width);
            for (auto const character : line) {
                m_tiles.push_back(parse_tile(character));
            }
        }
    }

    size_t run_neighbors() { return run(&Ferry::count_neighbors, 4); }

    size_t run_line_of_sight() { return run(&Ferry::count_line_of_sight, 5); }

private:
    size_t run(Counting_Function const counting_function, size_t const tolerance)
    {
        auto old_occupied_count{ evolve(counting_function, tolerance) };
        auto new_occupied_count{ evolve(counting_function, tolerance) };
        while (old_occupied_count != new_occupied_count) {
            old_occupied_count = new_occupied_count;
            new_occupied_count = evolve(counting_function, tolerance);
        }

        return new_occupied_count;
    }
    size_t evolve(Counting_Function neighbor_counting_function, size_t const tolerance)
    {
        size_t num_occupied_seats{};

        // temp : static to save some initializations
        static std::vector<Tile> new_tiles;
        new_tiles = m_tiles;

        size_t index{};
        for (auto const & tile : m_tiles) {
            switch (tile) {
            case Tile::occupied:
                if ((this->*neighbor_counting_function)(index) >= tolerance) {
                    new_tiles[index] = Tile::empty;
                } else {
                    ++num_occupied_seats;
                }
                break;
            case Tile::empty:
                if ((this->*neighbor_counting_function)(index) == 0) {
                    new_tiles[index] = Tile::occupied;
                    ++num_occupied_seats;
                }
                break;
            case Tile::floor:
                break;
            default:
                assert(false);
            }
            ++index;
        }

        m_tiles.swap(new_tiles);
        return num_occupied_seats;
    }

    size_t count_neighbors(size_t const index) const
    {
        auto const start_x{ index % m_width };
        auto const min_x{ start_x == 0 ? 0 : start_x - 1 };
        auto const max_x{ start_x + 1 == m_width ? start_x : start_x + 1 };
        auto const start_y{ index / m_width };
        auto const min_y{ start_y == 0 ? 0 : start_y - 1 };
        auto const max_y{ start_y + 1 == m_height ? start_y : start_y + 1 };
        auto const min_y_offset{ min_y * m_width };
        auto const max_y_offset{ max_y * m_width };

        size_t count{};

        for (auto y_offset{ min_y_offset }; y_offset <= max_y_offset; y_offset += m_width) {
            auto const max_neighbor_index{ y_offset + max_x };
            for (auto neighbor_index{ y_offset + min_x }; neighbor_index <= max_neighbor_index; ++neighbor_index) {
                if (neighbor_index == index) {
                    continue;
                }
                if (m_tiles[neighbor_index] == Tile::occupied) {
                    ++count;
                }
            }
        }

        return count;
    }
    size_t count_line_of_sight(size_t const index) const
    {
        size_t count{};

        auto const origin_x{ static_cast<int>(index % m_width) };
        auto const origin_y{ static_cast<int>(index / m_width) };

        for (auto y_direction{ -1 }; y_direction <= 1; ++y_direction) {
            for (auto x_direction{ -1 }; x_direction <= 1; ++x_direction) {
                if (x_direction == 0 && y_direction == 0) {
                    continue;
                }
                for (auto x{ origin_x + x_direction }, y{ origin_y + y_direction };
                     x >= 0 && x < m_width && y >= 0 && y < m_height;
                     x += x_direction, y += y_direction) {
                    auto const neighbor_index{ y * m_width + x };
                    auto const & tile{ m_tiles[neighbor_index] };
                    if (tile == Tile::occupied) {
                        ++count;
                        break;
                    }
                    if (tile == Tile::empty) {
                        break;
                    }
                    assert(tile == Tile::floor);
                }
            }
        }

        return count;
    }

    //==============================================================================
    static Tile parse_tile(char const character)
    {
        static constexpr char FLOOR = '.';
        static constexpr char EMPTY = 'L';

        if (character == FLOOR) {
            return Tile::floor;
        }

        assert(character == EMPTY);
        return Tile::empty;
    }
};

//==============================================================================
std::string day_11_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };

    Ferry ferry{ input };
    auto const number_of_occupied_seats{ ferry.run_neighbors() };

    return std::to_string(number_of_occupied_seats);
}

//==============================================================================
std::string day_11_b(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };

    Ferry ferry{ input };
    auto const number_of_occupied_seats{ ferry.run_line_of_sight() };

    return std::to_string(number_of_occupied_seats);
}
