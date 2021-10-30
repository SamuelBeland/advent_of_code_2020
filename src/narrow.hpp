#pragma once

#include <cassert>
#include <type_traits>

namespace aoc
{
namespace detail
{
#if defined(NDEBUG)
static constexpr auto IS_DEBUG = false;
#else
static constexpr auto IS_DEBUG = true;
#endif

} // namespace detail

//==============================================================================
template<typename To, typename From>
[[nodiscard]] constexpr To narrow(From const & from) noexcept(!detail::IS_DEBUG)
{
    if constexpr (detail::IS_DEBUG) {
        assert(std::is_signed_v<To> || from >= 0);
        To const newValue{ static_cast<To>(from) };
        assert(static_cast<From>(newValue) == from);
        return newValue;
    } else {
        return static_cast<To>(from);
    }
}

} // namespace aoc