//--- Day 17: Conway Cubes ---
//
// As your flight slowly drifts through the sky, the Elves at the Mythical Information Bureau at the North Pole contact
// you. They'd like some help debugging a malfunctioning experimental energy source aboard one of their super-secret
// imaging satellites.
//
// The experimental energy source is based on cutting-edge technology: a set of Conway Cubes contained in a pocket
// dimension! When you hear it's having problems, you can't help but agree to take a look.
//
// The pocket dimension contains an infinite 3-dimensional grid. At every integer 3-dimensional coordinate (x,y,z),
// there exists a single cube which is either active or inactive.
//
// In the initial state of the pocket dimension, almost all cubes start inactive. The only exception to this is a small
// flat region of cubes (your puzzle input); the cubes in this region start in the specified active (#) or inactive (.)
// state.
//
// The energy source then proceeds to boot up by executing six cycles.
//
// Each cube only ever considers its neighbors: any of the 26 other cubes where any of their coordinates differ by at
// most 1. For example, given the cube at x=1,y=2,z=3, its neighbors include the cube at x=2,y=2,z=2, the cube at
// x=0,y=2,z=3, and so on.
//
// During a cycle, all cubes simultaneously change their state according to the following rules:
//
// If a cube is active and exactly 2 or 3 of its neighbors are also active, the cube remains active. Otherwise, the cube
// becomes inactive. If a cube is inactive but exactly 3 of its neighbors are active, the cube becomes active.
// Otherwise, the cube remains inactive. The engineers responsible for this experimental energy source would like you to
// simulate the pocket dimension and determine what the configuration of cubes should be at the end of the six-cycle
// boot process.
//
// Even though the pocket dimension is 3-dimensional, this initial state represents a small 2-dimensional slice of it.
// (In particular, this initial state defines a 3x3x1 region of the 3-dimensional space.)
//
// After the full six-cycle boot process completes, 112 cubes are left in the active state.
//
// Starting with your given initial configuration, simulate six cycles. How many cubes are left in the active state
// after the sixth cycle?
//
// Your puzzle answer was 391.
//
//--- Part Two ---
// For some reason, your simulated results don't match what the experimental energy source engineers expected.
// Apparently, the pocket dimension actually has four spatial dimensions, not three.
//
// The pocket dimension contains an infinite 4-dimensional grid. At every integer 4-dimensional coordinate (x,y,z,w),
// there exists a single cube (really, a hypercube) which is still either active or inactive.
//
// Each cube only ever considers its neighbors: any of the 80 other cubes where any of their coordinates differ by at
// most 1. For example, given the cube at x=1,y=2,z=3,w=4, its neighbors include the cube at x=2,y=2,z=3,w=3, the cube
// at x=0,y=2,z=3,w=4, and so on.
//
// The initial state of the pocket dimension still consists of a small flat region of cubes. Furthermore, the same rules
// for cycle updating still apply: during each cycle, consider the number of active neighbors of each cube.
//
// For example, consider the same initial state as in the example above. Even though the pocket dimension is
// 4-dimensional, this initial state represents a small 2-dimensional slice of it. (In particular, this initial state
// defines a 3x3x1x1 region of the 4-dimensional space.)
//
// After the full six-cycle boot process completes, 848 cubes are left in the active state.
//
// Starting with your given initial configuration, simulate six cycles in a 4-dimensional space. How many cubes are left
// in the active state after the sixth cycle?
//
// Your puzzle answer was 2264.

#include "utils.hpp"

#include <resources.hpp>

namespace
{
//==============================================================================
constexpr std::size_t TOTAL_TICKS = 6;
constexpr std::size_t STARTING_LARGE = 8;
constexpr std::size_t STARTING_NARROW = 1;
constexpr std::size_t FINAL_LARGE = STARTING_LARGE + TOTAL_TICKS * 2 - 1;
constexpr std::size_t FINAL_NARROW = STARTING_NARROW + (TOTAL_TICKS + 1) * 2;

//==============================================================================
template<std::size_t Size, std::size_t... Sizes>
struct Dimensions {
    using next = Dimensions<Sizes...>;
    static constexpr std::size_t num_dimensions = next::num_dimensions + 1;
    static constexpr std::size_t size = Size;
    static constexpr std::size_t total_size = next::total_size * size;
    static constexpr std::size_t num_neighbors = next::num_neighbors * 3;
    static constexpr auto get_neighbor_offsets()
    {
        constexpr auto sub_dimensions_size{ next::total_size };
        std::array<std::size_t, num_neighbors> result{};
        constexpr std::array<std::size_t, 3> shifts{ 0, sub_dimensions_size, sub_dimensions_size * 2 };
        auto cur{ result.begin() };
        for (auto const & shift : shifts) {
            for (auto const & sub_shift : next::neighbor_offsets) {
                *cur++ = shift + sub_shift;
            }
        }
        return result;
    }
    static constexpr auto neighbor_offsets = get_neighbor_offsets();
};

//==============================================================================
template<std::size_t Size>
struct Dimensions<Size> {
    static_assert(Size >= 3);
    static constexpr std::size_t num_dimensions = 1;
    static constexpr std::size_t size = Size;
    static constexpr std::size_t total_size = Size;
    static constexpr std::size_t num_neighbors = 3;
    static constexpr std::array<std::size_t, num_neighbors> neighbor_offsets{ 0, 1, 2 };
};

//==============================================================================
template<typename T, std::size_t Size>
[[nodiscard]] constexpr std::array<T, Size - 1> remove_value(std::array<T, Size> const & arr,
                                                             std::size_t const value) noexcept
{
    std::array<T, Size - 1> result{};
    auto cur{ result.begin() };
    for (auto const & it : arr) {
        if (it == value) {
            continue;
        }
        *cur++ = it;
    }
    return result;
}

//==============================================================================
using cube_t = unsigned;

//==============================================================================
constexpr cube_t INACTIVE = 0;
constexpr cube_t ACTIVE = 1;

//==============================================================================
constexpr cube_t parse_state(char const c) noexcept(!aoc::detail::IS_DEBUG)
{
    assert(c == '.' || c == '#');
    return c == '#' ? ACTIVE : INACTIVE;
}

//==============================================================================
[[nodiscard]] constexpr cube_t compute_state(cube_t const & initial_state,
                                             cube_t const & number_of_active_neighbors) noexcept
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
template<typename Dimensions, typename It>
void insert_at(aoc::StringView const & string, It it)
{
    if constexpr (Dimensions::num_dimensions == 1) {
        // at num_dimensions == 1, the iterator is already offset for us
        std::transform(string.cbegin(), string.cend(), it, parse_state);
    }
    if constexpr (Dimensions::num_dimensions == 2) {
        // at num_dimensions == 2, the iterator is offset to the beginning of the plane
        auto const lines{ string.split('\n') };

        static constexpr auto LINE_SIZE = Dimensions::next::total_size;
        auto const baseOffset{ (Dimensions::size - lines.size()) / 2 * LINE_SIZE };
        it += baseOffset;
        auto const linesWidth{ lines.front().size() };
        assert(aoc::all_of(lines, [&](aoc::StringView const & line) { return line.size() == linesWidth; }));
        auto const linesOffset{ (Dimensions::next::size - linesWidth) / 2 };

        for (auto const & line : lines) {
            insert_at<Dimensions::next>(line, it + linesOffset);
            it += Dimensions::next::size;
        }
    }
    if constexpr (Dimensions::num_dimensions > 2) {
        // at num_dimensions > 2, we need to advance the iterator up to the beginning of the center plane
        static auto constexpr PLANES_TO_SKIP = (Dimensions::size - 1) / 2;
        static auto constexpr OFFSET = PLANES_TO_SKIP * Dimensions::next::total_size;
        insert_at<typename Dimensions::next>(string, it + OFFSET);
    }
}

//==============================================================================
template<typename Dimensions>
struct Space {
    std::array<cube_t, Dimensions::total_size> data;
    //==============================================================================
    [[nodiscard]] size_t num_active_cubes() const noexcept { return aoc::count(data, ACTIVE); }
    void tick(std::size_t const num_ticks) noexcept
    {
        for (std::size_t i{}; i < num_ticks; ++i) {
            tick_implementation();
        }
    }
    //==============================================================================
    void from_string(aoc::StringView const & string) noexcept
    {
        static_assert(Dimensions::num_dimensions >= 2);
        insert_at<Dimensions>(string, data.begin());
    }

private:
    //==============================================================================
    void tick_implementation() noexcept
    {
        decltype(data) sum_matrix{};

        static constexpr std::size_t AMOUNT_TO_REMOVE{ Dimensions::neighbor_offsets.back() };
        static constexpr std::size_t AMOUNT_TO_COPY{ Dimensions::total_size - AMOUNT_TO_REMOVE };
        static constexpr std::size_t MEDIAN_OFFSET{ Dimensions::neighbor_offsets[Dimensions::num_neighbors / 2] };
        static constexpr auto OFFSETS{ remove_value(Dimensions::neighbor_offsets, MEDIAN_OFFSET) };

        auto const dest{ sum_matrix.begin() + MEDIAN_OFFSET };

        for (auto const offset : OFFSETS) {
            auto const begin{ data.cbegin() + offset };
            auto const end{ begin + AMOUNT_TO_COPY };
            std::transform(begin, end, dest, dest, std::plus());
        }
        aoc::transform(data, sum_matrix, data, compute_state);
    }
};

//==============================================================================
template<std::size_t... Dims>
std::string day_17(char const * input_file_path)
{
    auto const space{ std::make_unique<Space<Dimensions<Dims...>>>() };
    auto const input{ aoc::read_file(input_file_path) };
    space->from_string(input);
    space->tick(TOTAL_TICKS);
    auto const num_active_cubes{ space->num_active_cubes() };

    return std::to_string(num_active_cubes);
}

} // namespace

//==============================================================================
std::string day_17_a(char const * input_file_path)
{
    return day_17<FINAL_NARROW, FINAL_LARGE, FINAL_LARGE>(input_file_path);
}

//==============================================================================
std::string day_17_b(char const * input_file_path)
{
    return day_17<FINAL_NARROW, FINAL_NARROW, FINAL_LARGE, FINAL_LARGE>(input_file_path);
}