# My Standard Library
My own implementation of some features in C/C++ standard library

## Achieved Features

### `tuple`

* Implementation

Basically, the tuple is implemented by **recursion** in design and **inheritance** in practice. Specifically, a base **empty** tuple struct is needed, and all the other **non-empty** tuples shall inherit from the base one recursively as follow:

```cpp
// base empty tuple
template <typename ...Ts>
struct tuple {...};

// generic non-empty tuple
template <typename T1, typename ...T2s>
struct tuple<T1, T2s...> : tuple<T2s...> {...}
```

As for visiting elements of the tuple, we need a handler to recursively **trait** the desired **sub-tuple** in order to extract the exact `val_`. This handler is achieved in struct `tuple_element`.

```cpp
// base tuple_element
template <typename tuple_>
struct tuple_element<0, tuple_>
{
    using type = tuple_; 
};

// generic tuple_element
template <int idx, typename tuple_>
struct tuple_element
{
    using type = typename tuple_element<idx - 1, ...>::type;
};
```

* Usage

My tuple provides all kinds of constructors, and allows you to visit each element of the tuple in a standard library alike method `get<i>()`. Comparison between two tuples through the operator `==` is also available. 

```cpp
tuple<int, char, std::string> t1(1, 'a', "hello world");
tuple<int, char, std::string> t2(2, 'b', "see u world");

cout << get<0>(t1) << endl;
cout << get<0>(t2) << endl;

cout << (t1 == t2) << endl;
```

For more detailed implementation and usage of the tuple, check `include\tuple.hpp` and `test\tuple.cpp`.