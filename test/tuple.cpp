#include "tuple.hpp"
#include <iostream>
#include <string>

auto get_tuple(int par1, char par2, std::string par3)
{
    return bstd::tuple<int, char, std::string>(par1, par2, par3);
}

int main(int argc, char* argv[])
{
    // create a tuple
    bstd::tuple<int, char, std::string> t1(1, 'a', "hello world");

    std::cout << "t1: ";
    std::cout << bstd::get<0>(t1) << " ";
    std::cout << bstd::get<1>(t1) << " ";
    std::cout << bstd::get<2>(t1) << " ";
    std::cout << std::endl;
    std::cout << std::endl;

    // copy constructor
    bstd::tuple<int, char, std::string> t2(t1);
    std::cout << "copy t1 into t2";
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "t2: ";
    std::cout << bstd::get<0>(t2) << " ";
    std::cout << bstd::get<1>(t2) << " ";
    std::cout << bstd::get<2>(t2) << " ";
    std::cout << std::endl;
    std::cout << std::endl;

    // "==" operator
    std::cout << "is t1 the same as t2 now: " << (t1 == t2);
    std::cout << std::endl;
    std::cout << std::endl;

    // use get() to modify the tuple
    bstd::get<0>(t2) = 2;
    bstd::get<1>(t2) = 'b';
    bstd::get<2>(t2) = "fucku world";

    std::cout << "after modification"; 
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "t2: ";
    std::cout << bstd::get<0>(t2) << " ";
    std::cout << bstd::get<1>(t2) << " ";
    std::cout << bstd::get<2>(t2) << " ";
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "is t1 the same as t2 now: " << (t1 == t2);
    std::cout << std::endl;
    std::cout << std::endl;

    // move constructor
    auto t3 = get_tuple(3, 'c', "see u world");

    std::cout << "t3: ";
    std::cout << bstd::get<0>(t3) << " ";
    std::cout << bstd::get<1>(t3) << " ";
    std::cout << bstd::get<2>(t3) << " ";
    std::cout << std::endl;
    std::cout << std::endl;

    return 0;
}