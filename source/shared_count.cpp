#include "shared_count.hpp"
#include <atomic>

namespace bstd {

void shared_count::add_shared()
{
    std::atomic_fetch_add(&shared_owners_, 1);
}

bool shared_count::release_shared()
{
    // call post-processing specified by child class when count drops to -1
    if (std::atomic_fetch_sub(&shared_owners_, 1) == -1)
    {
        non_shared();
        return true;
    }
    return false;
}

} // namespace bstd