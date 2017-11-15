#include "parsed.h"

#include <numeric>
#include <algorithm>

namespace saki
{



const util::Stactor<C34, 14> &Parsed::heads() const
{
    return mHeads;
}

int Parsed::work() const
{
    auto aux = [](int s, C34 c) { return s + c.work(); };
    return std::accumulate(mHeads.begin(), mHeads.end(), 0, aux);
}

/// \brief ordered equal, not set equal
bool Parsed::operator==(const Parsed &that) const
{
    if (mHeads.size() != that.mHeads.size())
        return false;

    return std::equal(mHeads.begin(), mHeads.end(), that.mHeads.begin());
}

void Parsed::append(C34 head)
{
    mHeads.pushBack(head);
}

void Parsed::sort()
{
    std::sort(mHeads.begin(), mHeads.end());
}



} // namespace saki


