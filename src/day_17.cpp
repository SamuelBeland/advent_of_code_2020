#include "utils.hpp"

#include <iterator>
#include <resources.hpp>

namespace
{
//==============================================================================
constexpr size_t MAX_STARTING_WIDTH = 8;
constexpr size_t MAX_TICKS = 5;
constexpr size_t MAX_WIDTH = MAX_STARTING_WIDTH + MAX_TICKS * 2;
constexpr size_t SLICE_SIZE = MAX_WIDTH * MAX_WIDTH;
constexpr size_t SPACE_SIZE = MAX_WIDTH * MAX_WIDTH * MAX_WIDTH;

//==============================================================================
template<typename T>
constexpr T constexpr_pow(T const & base, size_t exponent)
{
    T result{ 1 };
    while (exponent-- > 0) {
        result *= base;
    }
    return result;
}

//==============================================================================
template<size_t NUM_DIMENSIONS>
struct Num_Permutations {
    static constexpr auto value = constexpr_pow(size_t{ 3 }, NUM_DIMENSIONS) - 1;
};

//==============================================================================
using cube_t = unsigned;

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
template<size_t NUM_DIMENSIONS>
constexpr std::array<size_t, Num_Permutations<NUM_DIMENSIONS>::value> get_all_permutation_offsets()
{
    std::array<size_t, Num_Permutations<NUM_DIMENSIONS>::value> permutation_offsets{};

    auto cur{ permutation_offsets.begin() };

    for (size_t z{}; z < 3; ++z) {
        auto const z_offset{ z * SLICE_SIZE };
        for (size_t y{}; y < 3; ++y) {
            auto const y_z_offset{ z_offset + y * MAX_WIDTH };
            for (size_t x{}; x < 3; ++x) {
                if (z == 1 && y == 1 && x == 1) {
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
struct Space {
    std::array<cube_t, SPACE_SIZE> data;
    //==============================================================================
    [[nodiscard]] size_t num_active_cubes() const { return count(data, ACTIVE); }
    void tick(size_t num_ticks);
    //==============================================================================
    static std::unique_ptr<Space> from_string(std::string_view const & input);

private:
    //==============================================================================
    void tick_impl();
};

//==============================================================================
void Space::tick(size_t num_ticks)
{
    while (num_ticks-- > 0) {
        tick_impl();
    }
}

//==============================================================================
std::unique_ptr<Space> Space::from_string(std::string_view const & input)
{
    // Parse lines and make sure that they represent a square
    auto const lines{ split(input) };
    assert(!lines.empty());
    auto const width{ lines.front().size() };
    assert(all_of(lines, [&](std::string_view const & line) { return line.size() == width; }));

    // We only start with a single slice to parse. The parsed slice is going to be much smaller than the allocated
    // slice, so we should make sure to place it right in the center of our normal (bigger) slice.
    auto const width_leftover{ (MAX_WIDTH - width) / 2 };

    auto slice{ std::make_unique<std::array<cube_t, SLICE_SIZE>>() };
    auto const base_offset{ width_leftover + width_leftover * MAX_WIDTH };
    for (size_t line_index{}; line_index < lines.size(); ++line_index) {
        auto const offset{ base_offset + line_index * MAX_WIDTH };
        auto const & line{ lines[line_index] };
        std::transform(std::cbegin(line), std::cend(line), slice->data() + offset, parse_state);
    }

    auto space{ std::make_unique<Space>() };

    auto const offset{ MAX_WIDTH / 2 * SLICE_SIZE };
    std::copy_n(std::cbegin(*slice), SLICE_SIZE, std::begin(space->data) + offset);

    return space;
}

//==============================================================================
void Space::tick_impl()
{
    static constexpr auto COPY_OFFSET{ SLICE_SIZE + MAX_WIDTH + 1 };
    static constexpr auto AMOUNT_TO_COPY{ (MAX_WIDTH - 2) * (MAX_WIDTH - 2) * (MAX_WIDTH - 2) };
    static constexpr auto PERMUTATION_OFFSETS{ get_all_permutation_offsets<3>() };

    auto const data_to_copy{ data.cbegin() + COPY_OFFSET };
    auto const data_to_copy_end{ data_to_copy + AMOUNT_TO_COPY };
    auto const sum_matrix{ std::make_unique<Space>() };

    // compute permutations
    for (auto const & offset : PERMUTATION_OFFSETS) {
        std::transform(data_to_copy,
                       data_to_copy_end,
                       std::cbegin(sum_matrix->data) + offset,
                       std::begin(sum_matrix->data) + offset,
                       std::plus());
    }

    // compute state
    std::transform(std::cbegin(data), std::cend(data), std::cbegin(sum_matrix->data), std::begin(data), compute_state);
}
} // namespace

//==============================================================================
std::string day_17_a(char const * input_file_path)
{
    auto const input{ read_file(input_file_path) };
    auto const space{ Space::from_string(input) };
    space->tick(6);
    auto const num_active_cubes{ space->num_active_cubes() };

    return std::to_string(num_active_cubes);
}

//==============================================================================
std::string day_17_b(char const * input_file_path)
{
    return "coucou";
}