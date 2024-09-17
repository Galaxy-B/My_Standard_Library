#ifndef _MY_FUTURE_HPP_
#define _MY_FUTURE_HPP_
#include <condition_variable>
#include <exception>
#include <future>
#include <memory>
#include <mutex>
#include <type_traits>
#include "shared_count.hpp"

/********************************************************************

* This header file implements a standard-library alike future and promise

*********************************************************************/

namespace bstd {

// forward declaration of promise
template <class Ret>
class promise;

// forward declaration of future
template <class Ret>
class future;

enum associate_state
{
    initial  = 0,   
    attached = 1,   // future object has been attached to the channel
    filled   = 2,   // value of the async call has been filled in the channel 
    ready    = 4,   // the value in the channel is ready to be loaded 
    deferred = 8,   // solving will be deferred till a certain point
};

// exceptions that may throw when using future and promise
class future_exception : public std::exception
{
private:
    int exception_;

public:
    const static int channel_attached  = 1;
    const static int value_already_set = 2;
    const static int no_channel        = 4;
    const static int broken_promise    = 8;

    explicit future_exception(int exception) : exception_(exception) {}

    // print detailed info according to the given exception
    void what();
};

// maintain the state of the channel between promise and future
class channel_state : public shared_count
{
    template <class> friend class promise;
    template <class> friend class future;

protected:
    // store the exception thrown by the callee thread
    std::exception_ptr exception_;

    // make sure that the channel is thread-safe
    mutable std::mutex mutex_;
    mutable std::condition_variable cv_;

    // state of the channel
    unsigned state_{initial};

    // when the value is deferred, use callback to finish the async call
    // virtual void execute() = 0; [TODO]

    // attach a future to the channel
    void attach_future();

    // find whether the channel is readable (i.e. value is ready or exception is thrown)
    bool has_value() const;

    // wait if the value is not ready yet
    void wait_result(std::unique_lock<std::mutex>& lock);

    // set exception in the callee thread
    void set_exception(std::exception_ptr exception);
};

// maintain the value of the channel between promise and future
template <class Ret>
class channel_value : public channel_state
{
    using base = channel_state;
    // memory-aligned type of the return value of the channel
    using type = typename std::aligned_storage<sizeof(Ret), std::alignment_of<Ret>::value>::type;

    template <class> friend class promise;
    template <class> friend class future;

protected:
    type value_;

    // set return value of the channel (i.e. the async call has completed)
    template<class Arg>
    void set_value(Arg&& arg)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        // throw exception if there has been a result set
        if (has_value())
        {
            throw future_exception(future_exception::value_already_set);
        }

        // construct return value and modify the state of the channel
        ::new(&value_) Ret(std::forward<Arg>(arg));
        state_ |= filled | ready;
        cv_.notify_all();
    }
    
    // fetch return value of the channel
    Ret move()
    {
        std::unique_lock<std::mutex> lock(mutex_);

        // first check if the result is ready
        wait_result(lock);
        // then check if there is any exception
        if (exception_ != nullptr)
        {
            std::rethrow_exception(exception_);
        }

        return std::move(*reinterpret_cast<Ret*>(&value_));
    }

    // override of the virtual non_shared() of shared count class
    void non_shared()
    {
        // release the return value first (if there is one)
        if (state_ & filled)
        {
            reinterpret_cast<Ret*>(&value_)->~Ret();
        }

        // then release the channel itself
        delete this;
    } 
};

// funtor for releasing shared count
struct release_shared_count
{
    void operator()(shared_count* count) 
    {
        count->release_shared();
    }
};

// future  (i.e. the caller end of the channel)
template <class Ret>
class future
{
private:
    // channel of the promise-future pair
    channel_value<Ret>* channel_;

    // do not expose the constructor
    explicit future(channel_value<Ret>* channel) : channel_(channel) 
    {
        channel_->attach_future();
    }

    template <class> friend class promise;

public:
    // get the return value of the channel
    Ret get()
    {
        // make sure that the channel will be released after get()
        std::unique_ptr<shared_count, release_shared_count> _(channel_);
        
        // store the channel pointer before we set it to null
        channel_value<Ret>* channel = channel_;
        channel_ = nullptr;
        return channel->move();
    }

    // if the user didn't call get(), we have to release the channel manually
    ~future()
    {
        if (channel_)
        {
            channel_->release_shared();
        }
    }
};

// promise (i.e. the callee end of the channel)
template <class Ret>
class promise
{
private:
    // channel of the promise-future pair
    channel_value<Ret>* channel_;

    template <class> friend class std::packaged_task;

public:
    promise() : channel_(new channel_value<Ret>()) {}

    // get the future matching the promise
    future<Ret> get_future()
    {
        // check whether a channel has been constructed
        if (channel_ == nullptr)
        {
            throw future_exception(future_exception::no_channel);
        }

        return future<Ret>(channel_);
    }

    // set the return value of the channel [lvalue]
    void set_value(const Ret& value)
    {
        // check whether a channel has been constructed
        if (channel_ == nullptr)
        {
            throw future_exception(future_exception::no_channel);
        }

        channel_->set_value(value);
    }

    // set the return value of the channel [rvalue]
    void set_value(Ret&& value)
    {
        // check whether a channel has been constructed
        if (channel_ == nullptr)
        {
            throw future_exception(future_exception::no_channel);
        }

        channel_->set_value(value);
    }

    ~promise()
    {
        // release the channel if there is one unreleased
        if (channel_)
        {
            // if there is a associated future but no value is filled in the channel
            if (!channel_->has_value() && channel_->get_count() > 0)
            {
                channel_->set_exception(std::make_exception_ptr(future_exception(future_exception::broken_promise)));
            }

            channel_->release_shared();
        }
    }
};

} // namespace bstd

#endif