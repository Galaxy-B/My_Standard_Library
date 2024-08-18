#ifndef _MY_TUPLE_HPP_
#define _MY_TUPLE_HPP_

/********************************************************************

* This header file implements a standard-library alike tuple

*********************************************************************/

namespace bstd {

// base tuple struct
template <typename ...T>
struct tuple {};

// generically recursive tuple struct
template <typename T1, typename ...T2>
struct tuple<T1, T2...> : tuple<T2...>
{
    T1 val_;

    // constructors
    tuple() {}
    tuple(T1 val, T2... args) : val_(val), tuple<T2...>(args...) {}
};

} // namespace bstd

#endif