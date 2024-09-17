#include "future.hpp"
#include <chrono>
#include <numeric>
#include <vector>
#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
    // create a promise and fetch its matching future
    bstd::promise<int> promise;
    bstd::future<int> future = promise.get_future();

    // create a simple task which return result through promise
    auto task = [](std::vector<int>& data, bstd::promise<int>& promise)
    {
        // simulate this sum up into a heavy workload
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        int sum = std::accumulate(data.begin(), data.end(), 0);
        promise.set_value(sum);
    };

    // commit this task to a thread and use future to fetch the result
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::thread callee(task, std::ref(data), std::ref(promise));

    // doing other things in the caller
    std::cout << "doing our own business ..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "our own business completed" << std::endl;

    // use future to fetch the result of the previous async call
    std::cout << "result of async call is: " << future.get() << std::endl;

    // wait for the thread finishing
    callee.join();
    return 0;
}