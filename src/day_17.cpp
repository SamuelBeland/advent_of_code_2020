#include "utils.hpp"

#include <iterator>
#include <resources.hpp>

namespace
{
//==============================================================================
constexpr size_t WIDTH = 31;
constexpr size_t SLICE_SIZE = WIDTH * WIDTH;
constexpr size_t SPACE_SIZE = WIDTH * WIDTH * WIDTH;

constexpr size_t NUM_PERMUTATIONS = 3 * 3 * 3 - 1;

//==============================================================================
using cube_t = unsigned;

//==============================================================================
using Space = std::array<cube_t, SPACE_SIZE>;

//==============================================================================
constexpr cube_t INACTIVE = 0;
constexpr cube_t ACTIVE = 1;

//==============================================================================
constexpr cube_t parse_state(char const c)
{
    assert(c == '.' || c == '#');
    return c == '#' ? ACTIVE : INACTIVE;
}

//==============================================================================
std::unique_ptr<Space> parse_space(std::string_view const & input)
{
    // Parse lines and make sure that they represent a square
    auto const lines{ split(input) };
    assert(!lines.empty());
    auto const width{ lines.front().size() };
    assert(all_of(lines, [&](std::string_view const & line) { return line.size() == width; }));

    // We only start with a single slice to parse. The parsed slice is going to be much smaller than the allocated
    // slice, so we should make sure to place it right in the center of our normal (bigger) slice.
    auto const width_leftover{ (WIDTH - width) / 2 };

    auto slice{ std::make_unique<std::array<cube_t, SLICE_SIZE>>() };
    auto const base_offset{ width_leftover + width_leftover * WIDTH };
    for (size_t line_index{}; line_index < lines.size(); ++line_index) {
        auto const offset{ base_offset + line_index * WIDTH };
        auto const & line{ lines[line_index] };
        std::transform(std::cbegin(line), std::cend(line), slice->data() + offset, parse_state);
    }

    auto space{ std::make_unique<Space>() };

    auto const offset{ WIDTH / 2 * SLICE_SIZE };
    std::copy_n(std::cbegin(*slice), SLICE_SIZE, std::begin(*space) + offset);

    return space;
}

//==============================================================================
constexpr std::array<size_t, NUM_PERMUTATIONS> get_all_permutation_offsets()
{
    std::array<size_t, NUM_PERMUTATIONS> permutation_offsets{};

    auto cur{ permutation_offsets.begin() };

    for (size_t z{}; z < 3; ++z) {
        auto const z_offset{ z * SLICE_SIZE };
        for (size_t y{}; y < 3; ++y) {
            auto const y_z_offset{ z_offset + y * WIDTH };
            for (size_t x{}; x < 3; ++x) {
                if (z == 0 && y == 0 && x == 0) {
                    continue;
                }
                auto const offset{ y_z_offset + x };
                *cur++ = offset;
            }
        }
    }

    return permutation_offsets;
}

//==============================================================================
constexpr cube_t compute_state(cube_t const & initial_state, cube_t const & number_of_active_neighbors)
{
    if (number_of_active_neighbors == 3) {
        return ACTIVE;
    }

    if (number_of_active_neighbors == 2 && initial_state == ACTIVE) {
        return ACTIVE;
    }

    return INACTIVE;
}

//==============================================================================
void tick(Space & space)
{
    static constexpr auto COPY_OFFSET{ SLICE_SIZE + WIDTH + 1 };
    static constexpr auto AMOUNT_TO_COPY{ (WIDTH - 2) * (WIDTH - 2) * (WIDTH - 2) };
    static constexpr auto PERMUTATION_OFFSETS{ get_all_permutation_offsets() };

    auto const data_to_copy{ space.cbegin() + COPY_OFFSET };
    auto const data_to_copy_end{ data_to_copy + AMOUNT_TO_COPY };
    auto const sum_matrix{ std::make_unique<Space>() };

    // compute permutations
    for (auto const & offset : PERMUTATION_OFFSETS) {
        std::transform(data_to_copy,
                       data_to_copy_end,
                       std::cbegin(*sum_matrix) + offset,
                       std::begin(*sum_matrix) + offset,
                       std::plus());
    }

    // compute state
    std::transform(std::cbegin(space), std::cend(space), std::cbegin(*sum_matrix), std::begin(space), compute_state);
}

//==============================================================================
void tick(Space & space, size_t num_iterations)
{
    while (num_iterations > 0) {
        tick(space);
        --num_iterations;
    }
}

} // namespace

//==============================================================================
std::string day_17_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto space{ parse_space(input) };
    tick(*space, 6);
    auto const num_active_cubes{ count(*space, ACTIVE) };

    return std::to_string(num_active_cubes);
}

//==============================================================================
std::string day_17_b(char const * input_file_path)
{
    return "coucou";
}