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
int Parsed4::workOfHeads(const Parsed4::Heads &heads)
{
    auto aux = [](int s, C34 c) { return s + c.work(); };
    return std::accumulate(heads.begin(), heads.end(), 0, aux);
}

///
/// \brief Construct from raw comeld data
/// \param heads Heads in any order, must not have comeld overflow
/// \param barkCt Number of barks
///
/// By "comeld overflow", we mean the shanten number cannot
/// be calculated by simply counting comelds.
///
/// An example of comeld overflow: 112233m 12p 78p 45s 99s
/// (has 2 medls, 3 waiters, 1 birdhead --- too many waiters, must discard one)
///
Parsed4::Parsed4(const Parsed4::Heads &heads, int barkCt)
    : mHeads(heads)
    , mBarkCt(barkCt)
{
    std::sort(mHeads.begin(), mHeads.end());
}

const Parsed4::Heads &Parsed4::heads() const
{
    return mHeads;
}

int Parsed4::barkCt() const
{
    return mBarkCt;
}

///
/// \brief Compute 4-meld shanten number with given bark count
/// \param barkCt Number of barks
///
int Parsed4::step4() const
{
    return 8 - 2 * mBarkCt - workOfHeads(mHeads);
}

///
/// \brief Get the set of first-class effective tiles of this parse formation
///
std::bitset<34> Parsed4::effA4Set() const
{
    if (!mEffA4SetCache.has_value())
        computeEffA4();

    return *mEffA4SetCache;
}

util::Stactor<T34, 9> Parsed4::claim3sk() const
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
bool Parsed4::operator==(const Parsed4 &that) const
{
    if (mHeads.size() != that.mHeads.size() || mBarkCt != that.mBarkCt)
        return false;

    return std::equal(mHeads.begin(), mHeads.end(), that.mHeads.begin());
}

util::Stactor<T34, 9> Parsed4::minTilesTo(const std::array<C34, 3> &cs) const
{
    util::Stactor<T34, 9> res;
    for (const C34 &c : cs)
        res.pushBack(minTilesTo(c).range());

    return res;
}

util::Stactor<T34, 3> Parsed4::minTilesTo(const C34 &c) const
{
    // *INDENT-OFF*
    auto comp = [&c](const C34 &a, const C34 &b) {
        return a.tilesTo(c).size() < b.tilesTo(c).size();
    };
    // *INDENT-ON*

    return std::min_element(mHeads.begin(), mHeads.end(), comp)->tilesTo(c);
}

void Parsed4::computeEffA4() const
{
    std::bitset<34> res;

    auto is3 = [](C34 c) { return c.is3(); };
    auto isSeq2 = [](C34 c) { return c.isSeq2(); };
    auto isPair = [](C34 c) { return c.type() == C34::Type::PAIR; };

    const auto meldEnd = std::find_if_not(mHeads.begin(), mHeads.end(), is3);
    const auto seq2End = std::find_if_not(meldEnd, mHeads.end(), isSeq2);
    for (auto it = meldEnd; it != seq2End; ++it)
        for (T34 t : it->effA4())
            res.set(t.uId34());

    int faceCt = static_cast<int>(seq2End - mHeads.begin());
    const auto pairEnd = std::find_if_not(seq2End, mHeads.end(), isPair);
    int pairCt = static_cast<int>(pairEnd - seq2End);
    if (faceCt < 4 - mBarkCt) { // lacking of faces except pairs
        // regard all pair as triplet candidates
        // sacrificing bird-head candidates does not affect effA
        for (auto it = seq2End; it != pairEnd; ++it)
            res.set(it->head().uId34());

        // floating tiles affects effA only when
        // non-floats are less than '5 - barkCt'
        if (faceCt + pairCt < 5 - mBarkCt) {
            // regard all floating tiles as face or bird-head candidates
            for (auto it = pairEnd; it != mHeads.end(); ++it) {
                assert(it->type() == C34::Type::FREE);
                for (T34 t : it->effA4())
                    res.set(t.uId34());
            }
        }
    } else if (pairCt == 0) { // full face and isoride shape
        for (auto it = pairEnd; it != mHeads.end(); ++it)
            res.set(it->head().uId34());
    }

    mEffA4SetCache = res;
}

///
/// \brief Contruct from raw data
/// \param parseds Must have same shanten number for all element, w/o duplication
///
Parsed4s::Parsed4s(Parsed4s::Container &&parseds)
    : mParseds(parseds)
{
}

const Parsed4s::Container &Parsed4s::data() const
{
    return mParseds;
}

int Parsed4s::size() const
{
    return static_cast<int>(mParseds.size());
}

int Parsed4s::barkCt() const
{
    return mParseds.front().barkCt();
}

auto Parsed4s::begin() const -> Container::const_iterator
{
    return mParseds.begin();
}

auto Parsed4s::end() const -> Container::const_iterator
{
    return mParseds.end();
}

int Parsed4s::step4() const
{
    return mParseds.front().step4();
}



///
/// \brief Compute 4-meld first-class effective tiles
/// \return All 4-meld first-class effective tiles, sorted in ID-34 order
///
util::Stactor<T34, 34> Parsed4s::effA4() const
{
    return tiles34::toStactor(effA4Set());
}

std::bitset<34> Parsed4s::effA4Set() const
{
    if (!mEffA4SetCache.has_value()) {
        std::bitset<34> effA;
        for (const Parsed4 &p : mParseds)
            effA |= p.effA4Set();

        mEffA4SetCache = effA;
    }

    return *mEffA4SetCache;
}



///
/// \param plurals Tiles whose count in hand >= 2
/// \param floats Tiles whose count in hand == 1
///
Parsed7::Parsed7(const std::bitset<34> &plurals, const std::bitset<34> &floats)
    : mPlurals(plurals)
    , mFloats(floats)
    , mNeedKind(std::max(0, 7 - static_cast<int>(plurals.count() + floats.count())))
{
}

int Parsed7::step7() const
{
    return 6 - static_cast<int>(mPlurals.count()) + mNeedKind;
}

///
/// \brief Get set of 7-pair first-class effective tiles
///
std::bitset<34> Parsed7::effA7Set() const
{
    return mNeedKind == 0 ? mFloats : ~mPlurals;
}



///
/// \param yaos Presence of yao tiles in the hand. Non-yao bits must be zeros.
/// \param hasYaoPair True if a yao-pair is already in the hand
///
Parsed13::Parsed13(const std::bitset<34> &yaos, bool hasYaoPair)
    : mYaos(yaos)
    , mHasYaoPair(hasYaoPair)
{
}

int Parsed13::step13() const
{
    return 13 - static_cast<int>(mYaos.count()) - mHasYaoPair;
}

std::bitset<34> Parsed13::effA13Set() const
{
    if (!mEffA13SetCache.has_value()) {
        std::bitset<34> res;
        for (T34 t : tiles34::YAO13)
            if (!mHasYaoPair || !mYaos[t.uId34()])
                res.set(t.uId34());

        mEffA13SetCache = res;
    }

    return *mEffA13SetCache;
}



Parseds::Parseds(const Parsed4s &p4, const Parsed7 &p7, const Parsed13 &p13)
    : mParsed4s(p4)
    , mParsed7(p7)
    , mParsed13(p13)
{
}

Parseds::Parseds(const Parsed4s &p4)
    : mParsed4s(p4)
{
}

const Parsed4s &Parseds::get4s() const
{
    return mParsed4s;
}

int Parseds::step() const
{
    int minStep = mParsed4s.step4();

    if (mParsed4s.barkCt() == 0) {
        assert(mParsed7.has_value() && mParsed13.has_value());
        int step7 = mParsed7->step7();
        int step13 = mParsed13->step13();
        minStep = std::min(minStep, std::min(step7, step13));
    }

    return minStep;
}

int Parseds::step4() const
{
    return mParsed4s.step4();
}

int Parseds::step7() const
{
    return mParsed7.has_value() ? mParsed7->step7() : STEP_INF;
}

int Parseds::step13() const
{
    return mParsed13.has_value() ? mParsed13->step13() : STEP_INF;
}

util::Stactor<T34, 34> Parseds::effA() const
{
    return tiles34::toStactor(effASet());
}

util::Stactor<T34, 34> Parseds::effA4() const
{
    return mParsed4s.effA4();
}

std::bitset<34> Parseds::effASet() const
{
    if (!mEffASetCache.has_value())
        computeEffA();

    return *mEffASetCache;
}

std::bitset<34> Parseds::effA4Set() const
{
    return mParsed4s.effA4Set();
}

std::bitset<34> Parseds::effA7Set() const
{
    return mParsed7.has_value() ? mParsed7->effA7Set() : std::bitset<34>();
}

std::bitset<34> Parseds::effA13Set() const
{
    return mParsed13.has_value() ? mParsed13->effA13Set() : std::bitset<34>();
}

void Parseds::computeEffA() const
{
    if (mParsed4s.barkCt() > 0) {
        mEffASetCache = mParsed4s.effA4Set();
        return;
    }

    assert(mParsed7.has_value() && mParsed13.has_value());

    int step4 = mParsed4s.step4();
    int step7 = mParsed7->step7();
    int step13 = mParsed13->step13();
    int minStep = std::min(step4, std::min(step7, step13));

    std::bitset<34> res;

    if (step4 == minStep)
        res |= mParsed4s.effA4Set();

    if (step7 == minStep)
        res |= mParsed7->effA7Set();

    if (step13 == minStep)
        res |= mParsed13->effA13Set();

    mEffASetCache = res;
}



} // namespace saki
