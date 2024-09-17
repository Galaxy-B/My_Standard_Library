#include <iostream>
#include <mutex>
#include "future.hpp"

namespace bstd {

void future_exception::what()
{
    switch (exception_) 
    {
        case channel_attached:
            std::cout << "a future has been attached to the promise" << std::endl;
            break;
        case value_already_set:
            std::cout << "value in the future has already been set" << std::endl;
            break;
        case no_channel:
            std::cout << "the promise hasn't be associated to a channel" << std::endl;
            break;
        case broken_promise:
            std::cout << "the promise doesn't return anything before release" << std::endl;
            break;
        default:
            std::cout << "unkwown exception captured by future" << std::endl;
    }
}

void channel_state::attach_future()
{
    std::lock_guard<std::mutex> lock(mutex_);

    // throw exception if there have been a future attached to the channel
    if ((state_ & attached) != 0)
    {
        throw future_exception(future_exception::channel_attached);
    }
    // otherwise increase a referrence count on the channel
    else
    {
        add_shared();
        state_ |= attached;
    }
}

bool channel_state::has_value() const
{
    return (state_ & filled) || (exception_ != nullptr);
}

void channel_state::wait_result(std::unique_lock<std::mutex>& lock)
{
    // the result is ready now, just return
    if (state_ & ready)
        return;
    // the result is deferred, finish the work in current thread
    else if (state_ & deferred)
    {
        state_ &= ~deferred;
        lock.unlock();
        // execute(); [TODO]
    }
    // the result is not ready yet, wait in conditional variable
    else
    {
        while ((state_ & ready) == 0)
        {
            lock.unlock(); // [TODO]
            cv_.wait(lock);
        }
    }
}

void channel_state::set_exception(std::exception_ptr exception)
{
    exception_ = exception;
}

} // namespace bstd