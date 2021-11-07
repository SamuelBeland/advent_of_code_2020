#include "utils.hpp"

#include <resources.hpp>

namespace
{
//==============================================================================
constexpr std::size_t TOTAL_TICKS = 6;
constexpr std::size_t STARTING_LARGE = 8;
constexpr std::size_t STARTING_NARROW = 1;
constexpr std::size_t FINAL_LARGE = STARTING_LARGE + TOTAL_TICKS * 2;
constexpr std::size_t FINAL_NARROW = STARTING_NARROW + TOTAL_TICKS * 2;

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

template<typename T, typename U>
[[nodiscard]] constexpr bool contains(T const & collection, U const & value) noexcept
{
    for (auto const & it : collection) {
        if (it == value) {
            return true;
        }
    }
    return false;
}

//==============================================================================
template<typename T, std::size_t Size>
[[nodiscard]] constexpr std::array<T, Size - 1> remove_value(std::array<T, Size> const & arr,
                                                             std::size_t const value) noexcept
{
    assert(contains(arr, value));
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

        auto const dest{ sum_matrix.begin() };

        for (auto const offset : OFFSETS) {
            auto const begin{ data.cbegin() + offset };
            auto const end{ begin + AMOUNT_TO_COPY };
            std::transform(begin, end, dest, dest, std::plus());
        }
        aoc::transform(data, sum_matrix, data, compute_state);
    }
};

} // namespace

//==============================================================================
std::string day_17_a(char const * input_file_path)
{
    using dimensions_t = Dimensions<FINAL_NARROW, FINAL_LARGE, FINAL_LARGE>;
    Space<dimensions_t> space{};

    auto const input{ aoc::read_file(input_file_path) };
    space.from_string(input);
    space.tick(TOTAL_TICKS);
    auto const num_active_cubes{ space.num_active_cubes() };

    return std::to_string(num_active_cubes);
}

//==============================================================================
std::string day_17_b(char const * input_file_path)
{
    return "coucou";
}