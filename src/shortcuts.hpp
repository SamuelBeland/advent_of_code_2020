#pragma once

#include <algorithm>
#include <cassert>
#include <numeric>

namespace aoc
{
template<typename Coll, typename Fn>
void transform(Coll & coll, Fn const & fn)
{
    std::transform(coll.begin(), coll.end(), fn);
}

template<typename In, typename Dest, typename Fn>
void transform(In const & coll, Dest & dest, Fn const & fn)
{
    std::transform(coll.cbegin(), coll.cend(), dest.begin(), fn);
}

template<typename In1, typename In2, typename Dest, typename Fn>
void transform(In1 const & in1, In2 const & in2, Dest & dest, Fn const & fn)
{
    std::transform(in1.cbegin(), in1.cend(), in2.cbegin(), dest.begin(), fn);
}

template<typename Coll, typename T, typename Fn>
[[nodiscard]] auto reduce(Coll const & coll, T && init, Fn const & fn)
{
    return std::reduce(coll.cbegin(), coll.cend(), std::forward<T>(init), fn);
}

template<typename Coll, typename T, typename Transform_Fn, typename Reduce_Fn>
[[nodiscard]] auto
    transform_reduce(Coll const & coll, T && init, Transform_Fn const & transform_fn, Reduce_Fn const & reduce_fn)
{
    return std::transform_reduce(coll.cbegin(), coll.cend(), std::forward<T>(init), reduce_fn, transform_fn);
}

template<typename In1, typename In2, typename T, typename Transform_Fn, typename Reduce_Fn>
[[nodiscard]] auto transform_reduce(In1 const & in1,
                                    In2 const & in2,
                                    T && init,
                                    Transform_Fn const & transform_fn,
                                    Reduce_Fn const & reduce_fn)
{
    return std::transform_reduce(in1.cbegin(),
                                 in1.cend(),
                                 in2.cbegin(),
                                 std::forward<T>(init),
                                 reduce_fn,
                                 transform_fn);
}

template<typename Coll>
void sort(Coll & coll)
{
    std::sort(coll.begin(), coll.end());
}

template<typename Coll>
Coll sort(Coll && coll)
{
    auto moved{ std::move(coll) };
    std::sort(moved.begin(), moved.end());
    return moved;
}

template<typename Coll, typename T>
[[nodiscard]] auto count(Coll const & coll, T const & value)
{
    return std::count(coll.cbegin(), coll.cend(), value);
}

template<typename Coll, typename Pred>
[[nodiscard]] auto count_if(Coll const & coll, Pred const & pred)
{
    return std::count_if(coll.cbegin(), coll.cend(), pred);
}

template<typename Coll, typename Pred>
[[nodiscard]] bool all_of(Coll const & coll, Pred const & pred)
{
    return std::all_of(coll.cbegin(), coll.cend(), pred);
}

template<typename Coll, typename Fn>
void for_each(Coll const & coll, Fn const & fn)
{
    std::for_each(coll.cbegin(), coll.cend(), fn);
}

template<typename Coll, typename Fn>
void for_each(Coll & coll, Fn const & fn)
{
    std::for_each(coll.begin(), coll.end(), fn);
}

template<typename Coll>
[[nodiscard]] auto max_element(Coll const & coll)
{
    return std::max_element(coll.cbegin(), coll.cend());
}

template<typename Coll, typename Pred>
[[nodiscard]] auto find_if(Coll & coll, Pred const & pred)
{
    return std::find_if(coll.begin(), coll.end(), pred);
}

template<typename Coll>
[[nodiscard]] auto unique(Coll & coll)
{
    return std::unique(coll.begin(), coll.end());
}

template<typename Coll, typename Pred>
auto remove_if(Coll & coll, Pred const & pred)
{
    auto const new_end{ std::remove_if(coll.begin(), coll.end(), pred) };
    auto const num_removed{ std::distance(new_end, coll.end()) };
    coll.erase(new_end, coll.end());
    return num_removed;
}

template<typename Coll, typename T>
void iota(Coll & coll, T const & init)
{
    std::iota(coll.begin(), coll.end(), init);
}

template<typename Coll, typename Pred>
auto partition(Coll & coll, Pred const & pred)
{
    return std::partition(coll.begin(), coll.end(), pred);
}

template<typename Coll, typename Pred>
[[nodiscard]] auto none_of(Coll const & coll, Pred const & pred)
{
    return std::none_of(coll.cbegin(), coll.cend(), pred);
}

template<typename Coll, typename Pred>
[[nodiscard]] auto any_of(Coll const & coll, Pred const & pred)
{
    return std::any_of(coll.cbegin(), coll.cend(), pred);
}

template<typename Coll, typename T>
[[nodiscard]] auto lower_bound(Coll const & coll, T const & value)
{
    assert(std::is_sorted(coll.cbegin(), coll.cend()));
    return std::lower_bound(coll.cbegin(), coll.cend(), value);
}

template<typename Coll, typename T>
[[nodiscard]] auto upper_bound(Coll const & coll, T const & value)
{
    assert(std::is_sorted(coll.cbegin(), coll.cend()));
    return std::upper_bound(coll.cbegin(), coll.cend(), value);
}

} // namespace aoc