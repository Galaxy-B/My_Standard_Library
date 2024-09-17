#ifndef _MY_SHARED_COUNT_HPP_
#define _MY_SHARED_COUNT_HPP_
#include <cstdint>
#include <atomic>

/********************************************************************

* This header file implements a counter on shared referrence

*********************************************************************/

namespace bstd {

// counter on shared referrences
class shared_count
{
protected:
    std::atomic<int64_t> shared_owners_{0};

    shared_count(const shared_count&)            = delete;
    shared_count& operator=(const shared_count&) = delete;
    
    virtual ~shared_count() {}

    // called when shared count drops to -1 (i.e. no one holds referrence to it anymore)
    virtual void non_shared() = 0;

    // add a shared referrence to this counter
    void add_shared();

public:
    shared_count() {}

    // get the current count of shared referrences
    int get_count();

    // minus a shared referrence from this counter, return true when it drops to -1
    bool release_shared();
};

} // namespace bstd

#endif