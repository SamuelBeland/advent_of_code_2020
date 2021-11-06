#include "utils.hpp"

#include <iterator>
#include <memory>
#include <resources.hpp>

namespace
{
//==============================================================================
constexpr size_t STARTING_WIDTH = 8;
constexpr size_t TOTAL_TICKS = 5;
constexpr size_t FINAL_WIDTH = STARTING_WIDTH + TOTAL_TICKS * 2;

//==============================================================================
template<typename T>
constexpr T constexpr_pow(T const & base, size_t exponent) noexcept
{
    T result{ 1 };
    while (exponent-- > 0) {
        result *= base;
    }
    return result;
}

template<std::size_t...>
struct Dimensions {
};

template<std::size_t Size>
struct Dimensions {
    static_assert(Size >= 3);
    static constexpr std::size_t size = Size;
    static constexpr std::size_t total_size = Size;
    static constexpr std::size_t num_neighbors = 3;
    static constexpr std::array<std::size_t, num_neighbors> neighbor_offsets{ 0, 1, 2 };
};

template<std::size_t Size, std::size_t... Sizes>
struct Dimensions {
    using sub_dimensions = Dimensions<Sizes...>;
    static constexpr std::size_t size = Size;
    static constexpr std::size_t total_size = sub_dimensions::total_size * size;
    static constexpr std::size_t num_neighbors = sub_dimensions::num_neighbors * 3;
    static constexpr auto neighbor_offsets = get_neighbor_offsets();

    static constexpr auto get_neighbor_offsets()
    {
        static constexpr auto sub_dimensions_size{ aoc::narrow<std::ptrdiff_t>(sub_dimensions::total_size) };
        std::array<std::size_t, num_neighbors> result{};
        static constexpr std::array<std::size_t, 3> shifts{ 0, sub_dimensions_size, sub_dimensions_size * 2 };
        auto cur{ result.begin() };
        for (auto const & shift : shifts) {
            for (auto const & sub_shift : sub_dimensions::neighbor_offsets) {
                *cur++ = shift + sub_shift;
            }
        }
        return result;
    }
};

template<typename T, std::size_t Size>
[[nodiscard]] constexpr std::array<T, Size - 1> remove_value(std::array<T, Size> const & arr,
                                                             std::size_t const value) noexcept
{
    static_assert(Size > 0);
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
    [[nodiscard]] static Space from_string(aoc::StringView const & string) noexcept { string.count('\n'); }

private:
    void tick_implementation() noexcept
    {
        decltype(data) sum_matrix{};

        static constexpr std::size_t AMOUNT_TO_REMOVE{ Dimensions::neighbors.back() };
        static constexpr std::size_t AMOUNT_TO_COPY{ Dimensions::total_size - AMOUNT_TO_REMOVE };
        static constexpr std::size_t MIDPOINT{ Dimensions::neighbors[Dimensions::num_neighbors / 2] };
        static constexpr auto neighbors{ remove_value(Dimensions::neighbors, MIDPOINT) };

        for (auto const offset : neighbors) {
            auto const begin{ data.cbegin() + offset };
            auto const end{ begin + AMOUNT_TO_COPY };
            std::transform(begin, end, sum_matrix.begin() + MIDPOINT, std::plus());
        }
        aoc::transform(data, sum_matrix, data, compute_state);
    }
};

//==============================================================================
std::unique_ptr<Space> Space::from_string(aoc::StringView const & input)
{
    // Parse lines and make sure that they represent a square
    auto const lines{ input.split('\n') };
    assert(!lines.empty());
    auto const width{ lines.front().size() };
    assert(aoc::all_of(lines, [&](aoc::StringView const & line) { return line.size() == width; }));

    // We only start with a single slice to parse. The parsed slice is going to be much smaller than the allocated
    // slice, so we should make sure to place it right in the center of our normal (bigger) slice.
    auto const width_leftover{ (FINAL_WIDTH - width) / 2 };

    auto slice{ std::make_unique<std::array<cube_t, SLICE_SIZE>>() };
    auto const base_offset{ width_leftover + width_leftover * FINAL_WIDTH };
    for (size_t line_index{}; line_index < lines.size(); ++line_index) {
        auto const offset{ base_offset + line_index * FINAL_WIDTH };
        auto const & line{ lines[line_index] };
        std::transform(line.cbegin(), line.cend(), slice->data() + offset, parse_state);
    }

    auto space{ std::make_unique<Space>() };

    constexpr auto offset{ FINAL_WIDTH / 2 * SLICE_SIZE };
    std::copy_n(std::cbegin(*slice), SLICE_SIZE, std::begin(space->data) + offset);

    return space;
}

//==============================================================================
void Space::tick_impl()
{
    static constexpr auto COPY_OFFSET{ SLICE_SIZE + FINAL_WIDTH + 1 };
    static constexpr auto AMOUNT_TO_COPY{ (FINAL_WIDTH - 2) * (FINAL_WIDTH - 2) * (FINAL_WIDTH - 2) };
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
    auto const input{ aoc::read_file(input_file_path) };
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