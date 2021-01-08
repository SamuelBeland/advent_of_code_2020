#include "utils.hpp"

template<typename Operation>
class Transformation
{
    Operation m_op;

public:
    explicit Transformation(Operation && op) : m_op(std::forward<Operation>(op)) {}

    template<typename T>
    auto operator()(T const & value) const
    {
        return m_op(value);
    }
};

template<typename Coll, typename Transformation>
class Transformed_View
{
    Coll const & m_coll;
    Transformation m_transformation;

public:
    class const_iterator
    {
        typename Coll::const_iterator m_it;
        Transformation const & m_transformation;

    public:
        using iterator_category = typename Coll::iterator::iterator_category;
        using value_type = typename Coll::iterator::value_type;
        using difference_type = typename Coll::iterator::difference_type;
        using pointer = typename Coll::iterator::pointer;
        using reference = typename Coll::iterator::reference;

        const_iterator(typename Coll::const_iterator && it, Transformation const & transformation)
            : m_it(std::forward<typename Coll::const_iterator>(it))
            , m_transformation(transformation)
        {
        }

        auto operator*() const { return m_transformation(*m_it); }
        bool operator!=(const_iterator const & other) const { return m_it != other.m_it; }
        const_iterator & operator++()
        {
            ++m_it;
            return *this;
        }
    };

    Transformed_View() = delete;
    Transformed_View(Coll const & coll, Transformation && transformation)
        : m_coll(coll)
        , m_transformation(std::forward<Transformation>(transformation))
    {
    }

    const_iterator cbegin() const { return const_iterator{ m_coll.cbegin(), m_transformation }; }
    const_iterator cend() const { return const_iterator{ m_coll.cend(), m_transformation }; }
};

namespace views
{
template<typename Unary_Op>
auto transform(Unary_Op && unary_op)
{
    return Transformation<Unary_Op>{ std::forward<Unary_Op>(unary_op) };
}
} // namespace views

template<typename Coll, typename Transformation>
auto operator|(Coll const & coll, Transformation && transformation)
{
    return Transformed_View{ coll, std::forward<Transformation>(transformation) };
}