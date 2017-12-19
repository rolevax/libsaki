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

util::Stactor<T34, 9> Parsed::claim3sk() const
{
    using namespace tiles34;

    // default value if no number tile in hand
    util::Stactor<T34, 9> res {
        6_m, 7_m, 8_m,
        6_p, 7_p, 8_p,
        6_s, 7_s, 8_s
    };

    for (int begin = 1; begin <= 7; begin++) {
        std::array<C34, 3> tars {
            C34(C34::Type::SEQ, T34(Suit::M, begin)),
            C34(C34::Type::SEQ, T34(Suit::P, begin)),
            C34(C34::Type::SEQ, T34(Suit::S, begin))
        };

        auto cand = minTilesTo(tars);
        if (cand.size() < res.size())
            res = cand;
    }

    return res;
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

util::Stactor<T34, 9> Parsed::minTilesTo(const std::array<C34, 3> &cs) const
{
    util::Stactor<T34, 9> res;
    for (const C34 &c : cs)
        res.pushBack(minTilesTo(c).range());

    return res;
}

util::Stactor<T34, 3> Parsed::minTilesTo(const C34 &c) const
{
    // *INDENT-OFF*
    auto comp = [&c](const C34 &a, const C34 &b) {
        return a.tilesTo(c).size() < b.tilesTo(c).size();
    };
    // *INDENT-ON*

    return std::min_element(mHeads.begin(), mHeads.end(), comp)->tilesTo(c);
}



} // namespace saki
