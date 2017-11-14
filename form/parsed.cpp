#include "parsed.h"

#include <numeric>

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

void Parsed::append(C34 head)
{
    mHeads.pushBack(head);
}



} // namespace saki


