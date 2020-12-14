#include "utils.hpp"
#include <resources.hpp>

//==============================================================================
class Ferry
{
public:
    enum class Tile { floor, empty, occupied };

    class Neighbor_Iterator
    {
    public:
    };

private:
    //==============================================================================
    size_t m_width;
    size_t m_width_including_buffers;
    size_t m_height;
    size_t m_height_including_buffers;
    std::vector<Tile> m_tiles;

public:
    //==============================================================================
    Ferry(std::string const & input)
    {
        auto const lines{ split(input) };

        m_width = lines.front().size();
        m_width_including_buffers = m_width + 2;
        m_height = lines.size();
        m_height_including_buffers = m_height + 2;

        auto const size_including_buffers{ m_width_including_buffers * m_height_including_buffers };

        m_tiles.reserve(size_including_buffers);

        m_tiles.resize(m_width_including_buffers, Tile::floor);
        for (auto const & line : lines) {
            assert(line.size() == m_width);
            m_tiles.push_back(Tile::floor);
            for (auto const character : line) {
                m_tiles.push_back(parse_tile(character));
            }
            m_tiles.push_back(Tile::floor);
        }
        m_tiles.resize(size_including_buffers, Tile::floor);
    }

    // returns the number of seats the changed status.
    size_t run()
    {
        size_t number_of_tiles_changes{};
        // temp : static to save some initializations
        static std::vector<Tile> new_tiles;
        new_tiles = m_tiles;

        auto read_it{ m_tiles.cbegin() + m_width_including_buffers + 1 };
        std::array<std::vector<Tile>::const_iterator, 8> neighbors{ read_it - m_width_including_buffers - 1,
                                                                    read_it - m_width_including_buffers,
                                                                    read_it - m_width_including_buffers + 1,
                                                                    read_it - 1,
                                                                    read_it + 1,
                                                                    read_it + m_width_including_buffers - 1,
                                                                    read_it + m_width_including_buffers,
                                                                    read_it + m_width_including_buffers + 1 };
        auto const increment_neighbors = [&neighbors]() {
            for (auto & neighbor : neighbors) {
                ++neighbor;
            }
        };
        auto const is_occupied
            = [](std::vector<Tile>::const_iterator const & neighbor) -> bool { return *neighbor == Tile::occupied; };

        auto write_it{ new_tiles.begin() + m_width_including_buffers + 1 };
        auto const read_end{ m_tiles.cend() - m_width_including_buffers - 1 };

        for (; read_it < read_end; ++read_it, ++write_it, increment_neighbors()) {
            if (*read_it == Tile::floor) {
                continue;
            }
            if (*read_it == Tile::empty) {
                if (std::none_of(neighbors.cbegin(), neighbors.cend(), is_occupied)) {
                    *write_it = Tile::occupied;
                    ++number_of_tiles_changes;
                }
            } else {
                assert(*read_it == Tile::occupied);
                auto const neighbors_count{ std::count_if(neighbors.cbegin(), neighbors.cend(), is_occupied) };
                if (neighbors_count >= 4) {
                    *write_it = Tile::empty;
                    ++number_of_tiles_changes;
                }
            }
        }

        m_tiles.swap(new_tiles);
        return number_of_tiles_changes;
    }

    size_t get_num_occupied_seats() const { return std::count(m_tiles.cbegin(), m_tiles.cend(), Tile::occupied); }

private:
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
    auto number_of_seats_changes{ ferry.run() };
    while (number_of_seats_changes > 0) {
        number_of_seats_changes = ferry.run();
    }

    auto const num_occupied_seats{ ferry.get_num_occupied_seats() };

    return std::to_string(num_occupied_seats);
}

//==============================================================================
std::string day_11_b(char const * input_file_path)
{
    return "day_11_a not implemented";
}
