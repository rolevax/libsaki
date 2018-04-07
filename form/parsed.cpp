#include "parsed.h"

#include <numeric>
#include <algorithm>

namespace saki
{



///
/// \brief Shanten number by which this parsing result decreases to the hand
/// \param heads Heads in any order
///
/// The function does not care about the 4-meld limit
///
int Parsed::workOfHeads(const Parsed::Heads &heads)
{
    auto aux = [](int s, C34 c) { return s + c.work(); };
    return std::accumulate(heads.begin(), heads.end(), 0, aux);
}

///
/// \brief Construct from raw comeld data
/// \param heads Heads in any order, must not have comeld overflow
///
/// By "comeld overflow", we mean the shanten number cannot
/// be calculated by simply counting comelds.
///
/// An example of comeld overflow: 112233m 12p 78p 45s 99s
/// (has 2 medls, 3 waiters, 1 birdhead --- too many waiters, must discard one)
///
Parsed::Parsed(const Parsed::Heads &heads)
    : mHeads(heads)
{
    std::sort(mHeads.begin(), mHeads.end());
}

const Parsed::Heads &Parsed::heads() const
{
    return mHeads;
}

///
/// \brief Compute 4-meld shanten number with given bark count
/// \param barkCt Number of barks
///
int Parsed::step4(int barkCt) const
{
    return 8 - 2 * barkCt - workOfHeads(mHeads);
}

///
/// \brief Get the set of first-class effective tiles of this parse formation
///
std::bitset<34> Parsed::effA4() const
{
    std::bitset<34> res;

    auto is3 = [](C34 c) { return c.is3(); };
    auto isSeq2 = [](C34 c) { return c.isSeq2(); };
    auto isPair = [](C34 c) { return c.type() == C34::Type::PAIR; };

    const auto meldEnd = std::find_if_not(mHeads.begin(), mHeads.end(), is3);
    const auto seq2End = std::find_if_not(meldEnd, mHeads.end(), isSeq2);
    for (auto it = meldEnd; it != seq2End; ++it)
        for (T34 t : it->effA4())
            res.set(t.id34());

    int faceCt = seq2End - mHeads.begin();
    const auto pairEnd = std::find_if_not(seq2End, mHeads.end(), isPair);
    if (faceCt < 4) { // lacking of faces except pairs
        // regard all pair as bibump waiters
        // sacrificing bird-head candidates does not harm effA
        for (auto it = seq2End; it != pairEnd; ++it)
            res.set(it->head().id34());

        // regard all floating tiles as face-seed
        // isoride cases are implied
        for (auto it = pairEnd; it != mHeads.end(); ++it) {
            assert(it->type() == C34::Type::FREE);
            for (T34 t : it->effA4())
                res.set(t.id34());
        }
    } else { // full faces
        bool hasBirdHead = pairEnd - seq2End != 0;
        if (!hasBirdHead) // isoride shape
            for (auto it = pairEnd; it != mHeads.end(); ++it)
                res.set(it->head().id34());
    }

    return res;
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
