#ifndef _MY_TUPLE_HPP_
#define _MY_TUPLE_HPP_

/********************************************************************

* This header file implements a standard-library alike tuple

*********************************************************************/

namespace bstd {

// base tuple struct
template <typename ...T>
struct tuple 
{
    // base comparison of empty tuple
    template <typename... Ts>
    bool equal(const tuple<>& obj) const
    {
        return true;
    }
};

// generically recursive tuple struct
template <typename T1, typename ...T2s>
struct tuple<T1, T2s...> : tuple<T2s...>
{
    T1 val_;

    using base = tuple<T2s...>;
    using self = tuple<T1, T2s...>;

    tuple() {}
    tuple(T1 val, T2s... args) : val_(val), base(args...)  {}
    tuple(const self& obj)     : val_(obj.val_), base(obj) {}
    tuple(self&& obj)          : val_(obj.val_), base(obj) {} 

    // fetch base struct of this tuple
    base& get_base()
    {
        return static_cast<base&>(*this);
    }

    const base& get_base() const
    {
        return static_cast<const base&>(*this);
    }

    // recursively compare this tuple with a given tuple
    template <typename... Ts>
    bool equal(const tuple<Ts...>& obj) const
    {
        return this->val_ == obj.val_ && base::equal(obj.get_base());
    }
};

// generic struct for recursively fetching each element in the tuple
template <int idx, typename tuple_>
struct tuple_element
{
    using type = typename tuple_element<idx - 1, typename tuple_::base>::type;
};

// base element struct
template <typename tuple_>
struct tuple_element<0, tuple_>
{
    using type = tuple_; 
};

// interface for fetching each value in the tuple
template <int idx, typename tuple_>
constexpr auto& get(tuple_& obj)
{
    using type = typename tuple_element<idx, tuple_>::type;
    return static_cast<type&>(obj).val_;
}

// overload of "=="
template <typename... Ts1, typename... Ts2>
bool operator==(const tuple<Ts1...>& obj1, const tuple<Ts2...>& obj2)
{
    return obj1.equal(obj2);
}

} // namespace bstd

#endif