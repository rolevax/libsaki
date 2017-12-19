#include "tile_count.h"
#include "../util/misc.h"



namespace saki
{



TileCount::TileCount()
{
    mCounts.fill(0);
    mAka5s.fill(0);
}

TileCount::TileCount(TileCount::AkadoraCount fillMode)
{
    mCounts.fill(4);
    mAka5s.fill(0);

    using namespace tiles37;

    switch (fillMode) {
    case AKADORA0:
        // do nothing
        break;
    case AKADORA3:
        inc(5_m, -1);
        inc(5_p, -1);
        inc(5_s, -1);
        inc(0_m, 1);
        inc(0_p, 1);
        inc(0_s, 1);
        break;
    case AKADORA4:
        inc(5_m, -1);
        inc(5_p, -2);
        inc(5_s, -1);
        inc(0_m, 1);
        inc(0_p, 2);
        inc(0_s, 1);
        break;
    }
}

TileCount::TileCount(std::initializer_list<T37> t37s)
{
    mCounts.fill(0);
    mAka5s.fill(0);

    for (const T37 &t : t37s)
        inc(t, 1);
}

int TileCount::ct(T34 t) const
{
    return mCounts[t.id34()];
}

int TileCount::ct(const T37 &t) const
{
    if (t.val() == 5) {
        int red = mAka5s[static_cast<int>(t.suit())];
        return t.isAka5() ? red : mCounts[t.id34()] - red;
    }

    return mCounts[t.id34()];
}

int TileCount::ctAka5() const
{
    return std::accumulate(mAka5s.begin(), mAka5s.end(), 0);
}

int TileCount::ctZ() const
{
    return std::accumulate(mCounts.begin() + 27, mCounts.end(), 0);
}

int TileCount::ctYao() const
{
    using namespace tiles34;
    return ct(1_m) + ct(9_m) + ct(1_p) + ct(9_p) + ct(1_s) + ct(9_s) + ctZ();
}

bool TileCount::hasZ() const
{
    return std::any_of(mCounts.begin() + 27, mCounts.end(), [](int i) { return i > 0; });
}

bool TileCount::hasYao() const
{
    using namespace tiles34;
    return ct(1_m) > 0 || ct(9_m) > 0
           || ct(1_p) > 0 || ct(9_p) > 0
           || ct(1_s) > 0 || ct(9_s) > 0
           || hasZ();
}

void TileCount::inc(const T37 &t, int delta)
{
    assert(ct(t) + delta >= 0);

    mCounts[t.id34()] += delta;
    if (t.isAka5())
        mAka5s[static_cast<int>(t.suit())] += delta;
}

TileCount &TileCount::operator-=(const TileCount &rhs)
{
    for (int ti = 0; ti < 34; ti++) {
        mCounts[ti] -= rhs.mCounts[ti];
        assert(mCounts[ti] >= 0);
    }

    for (int s = 0; s < 3; s++) {
        mAka5s[s] -= rhs.mAka5s[s];
        assert(mAka5s[s] >= 0);
    }

    return *this;
}

int TileCount::step(int barkCt) const
{
    int s4 = step4(barkCt);
    int s7 = step7();
    int s13 = step13();
    return std::min(s4, std::min(s7, s13));
}

int TileCount::stepGb(int barkCt) const
{
    int s4 = step4(barkCt);
    int s7 = step7Gb();
    int s13 = step13();
    return std::min(s4, std::min(s7, s13));
}

int TileCount::step4(int barkCt) const
{
    int maxCut = 4 - barkCt;
    int min = 8;

    // having-birdhead case
    for (int ti = 0; ti < 34; ti++) {
        if (mCounts[ti] >= 2) {
            T34Delta guard(mutableCounts(), T34(ti), -2);
            (void) guard;
            int comin = 7 - cutMeld(0, maxCut) - 2 * barkCt;
            min = std::min(min, comin);
        }
    }

    // birdhead-less case
    int comin = 8 - cutMeld(0, maxCut) - 2 * barkCt;
    min = std::min(min, comin);

    return min;
}

int TileCount::step7() const
{
    int pair = 0;
    int needKind = 7;

    for (int ti = 0; ti < 34; ti++) {
        if (mCounts[ti] > 0) {
            needKind--;
            if (mCounts[ti] >= 2)
                pair++;
        }
    }

    if (needKind <= 0)
        return 6 - pair;
    else
        return (6 - pair) + needKind;
}

int TileCount::step7Gb() const
{
    int pair = 0;
    for (int ti = 0; ti < 34; ti++)
        pair += (mCounts[ti] / 2);

    return 6 - pair;
}

int TileCount::step13() const
{
    int res = 13;
    bool gotPair = false;

    for (T34 t : tiles34::YAO13) {
        if (mCounts[t.id34()] > 0) {
            res--;
            if (!gotPair && mCounts[t.id34()] >= 2) {
                gotPair = true;
                res--;
            }
        }
    }

    return res;
}

bool TileCount::hasEffA(int barkCt, T34 t) const
{
    int curr = step(barkCt); // should use common min, not individual
    return (!dislike4(t) && peekDraw(t, &TileCount::step4, barkCt) < curr)
           || (barkCt == 0 && peekDraw(t, &TileCount::step7) < curr)
           || (barkCt == 0 && t.isYao() && peekDraw(t, &TileCount::step13) < curr);
}

bool TileCount::hasEffA4(int barkCt, T34 t) const
{
    return !dislike4(t) && peekDraw(t, &TileCount::step4, barkCt) < step4(barkCt);
}

bool TileCount::hasEffA7(T34 t) const
{
    return peekDraw(t, &TileCount::step7) < step7();
}

bool TileCount::hasEffA13(T34 t) const
{
    return t.isYao() && peekDraw(t, &TileCount::step13) < step13();
}

util::Stactor<T34, 34> TileCount::effA(int barkCt) const
{
    util::Stactor<T34, 34> res;

    for (T34 t : tiles34::ALL34)
        if (hasEffA(barkCt, t))
            res.pushBack(t);

    return res;
}

util::Stactor<T34, 34> TileCount::effA4(int barkCt) const
{
    util::Stactor<T34, 34> res;

    for (int ti = 0; ti < 34; ti++)
        if (hasEffA4(barkCt, T34(ti)))
            res.pushBack(T34(ti));

    return res;
}

util::Stactor<T34, 13> TileCount::t34s13() const
{
    util::Stactor<T34, 13> res;

    for (T34 t : tiles34::ALL34)
        if (ct(t) > 0)
            res.pushBack(t);

    return res;
}

util::Stactor<T37, 13> TileCount::t37s13(bool allowDup) const
{
    util::Stactor<T37, 13> res;

    for (const T37 &t : tiles37::ORDER37) {
        int num = ct(t);
        if (allowDup) {
            while (num-- > 0)
                res.pushBack(t);
        } else {
            if (num != 0)
                res.pushBack(t);
        }
    }

    return res;
}

// 't' will be a floating tile (form-4) if added to this 'count'
// a disliked tile might be 'useful' when there is a pure-empty ready
// but logically that's still not 'effective-A'
// since it does not change the step
bool TileCount::dislike4(T34 t) const
{
    int ti = t.id34();

    if (mCounts[ti] > 0) // can make a pair
        return false;

    if (t.isNum()) {
        if (t.val() >= 2 && mCounts[ti - 1] > 0)
            return false; // has left neighbor

        if (t.val() <= 8 && mCounts[ti + 1] > 0)
            return false; // has right neighbor

        if (t.val() >= 3 && mCounts[ti - 2] > 0)
            return false; // has left-left neighbor

        if (t.val() <= 7 && mCounts[ti + 2] > 0)
            return false; // has right-right neighbor

    }

    return true; // nobody likes this tile
}

std::vector<Parsed> TileCount::parse4(int barkCt) const
{
    std::vector<Parsed> reses;

    int maxCut = 4 - barkCt;
    int min = 8;

    // *INDENT-OFF*
    auto update = [&](bool hasBirdHead, T34 h = T34()) {
        auto subreses = cutMeldOut(0, maxCut);
        int comin = (hasBirdHead ? 7 : 8) - subreses[0].work() - 2 * barkCt;

        if (comin <= min) {
            if (comin < min) {
                min = comin;
                reses.clear();
            }

            if (hasBirdHead)
                for (Parsed &p : subreses)
                    p.append(C34(C34::Type::PAIR, h));

            for (Parsed &p : subreses) {
                p.sort();
                if (!util::has(reses, p))
                    reses.push_back(p);
            }
        }
    };
    // *INDENT-ON*

    // having-birdhead cases
    for (T34 h : tiles34::ALL34) {
        if (ct(h) >= 2) {
            T34Delta guard(mutableCounts(), h, -2);
            (void) guard;
            update(true, h);
        }
    }

    // birdhead-less case
    update(false);

    return reses;
}

std::vector<TileCount::Explain4Closed> TileCount::explain4(T34 pick) const
{
    // no assertion. the result will be illegal if the input is illegal

    T34Delta guard(mutableCounts(), pick, 1);
    (void) guard;

    std::vector<Explain4Closed> res;

    // enumerate for all possible birdheads
    for (int ti = 0; ti < 34; ti++) {
        if (mCounts[ti] >= 2) {
            T34Delta guard(mutableCounts(), T34(ti), -2);
            (void) guard;

            T34 pick(ti);
            Explain4Closed exp(pick);
            if (decomposeBirdless4(exp, mCounts)) {
                res.push_back(exp);

                // 111-222-333 --> 123-123-123
                // *INDENT-OFF*
                auto checkSequences = [](T34 l, T34 m, T34 r) -> bool {
                    return (l | m) && (m | r);
                };
                // *INDENT-ON*
                if (exp.triplets.size() == 3) { // 0/1 seq + 3 tri --> 3/4 seq
                    if (checkSequences(exp.triplets[0], exp.triplets[1], exp.triplets[2])) {
                        Explain4Closed exp2(pick);
                        if (exp.sequences.size() == 1 && exp.sequences[0] < exp.triplets[0])
                            exp2.sequences.push_back(exp.sequences[0]); // keep ordered

                        for (int i = 0; i < 3; i++)
                            exp2.sequences.push_back(exp.triplets[0]);

                        if (exp.sequences.size() == 1 && exp2.sequences.size() == 3)
                            exp2.sequences.push_back(exp.sequences[0]); // keep ordered

                        res.push_back(exp2);
                    }
                } else if (exp.triplets.size() == 4) { // 4 tri --> 3 seq + 1 tri
                    if (checkSequences(exp.triplets[0], exp.triplets[1], exp.triplets[2])) {
                        Explain4Closed exp3(pick);
                        for (int i = 0; i < 3; i++)
                            exp3.sequences.push_back(exp.triplets[0]);

                        exp3.triplets.push_back(exp.triplets[3]);
                        res.push_back(exp3);
                    }

                    if (checkSequences(exp.triplets[1], exp.triplets[2], exp.triplets[3])) {
                        Explain4Closed exp4(pick);
                        exp4.triplets.push_back(exp.triplets[0]);
                        for (int i = 1; i < 4; i++)
                            exp4.sequences.push_back(exp.triplets[1]);

                        res.push_back(exp4);
                    }
                }
            }
        }
    }

    assert(!res.empty());
    return res;
}

bool TileCount::onlyInTriplet(T34 pick, int barkCt) const
{
    assert(step(barkCt) == 0); // this algo does not work when step is -1
    assert(mCounts[pick.id34()] == 3);

    if (pick.isZ())
        return true;

    if (pick.val() <= 7
        && mCounts[pick.next().id34()] != 0
        && mCounts[pick.nnext().id34()] != 0) {
        T34Delta guard1(mutableCounts(), pick, -1);
        T34Delta guard2(mutableCounts(), pick.next(), -1);
        T34Delta guard3(mutableCounts(), pick.nnext(), -1);
        (void) guard1; (void) guard2; (void) guard3;

        int seqStep = step(barkCt + 1);
        if (seqStep == 0)
            return false;
    }

    if (pick.val() >= 3
        && mCounts[pick.prev().id34()] != 0
        && mCounts[pick.pprev().id34()] != 0) {
        T34Delta guard1(mutableCounts(), pick, -1);
        T34Delta guard2(mutableCounts(), pick.prev(), -1);
        T34Delta guard3(mutableCounts(), pick.pprev(), -1);
        (void) guard1; (void) guard2; (void) guard3;

        int seqStep = step(barkCt + 1);
        if (seqStep == 0)
            return false;
    }

    if (2 <= pick.val() && pick.val() <= 8
        && mCounts[pick.prev().id34()] != 0
        && mCounts[pick.next().id34()] != 0) {
        T34Delta guard1(mutableCounts(), pick, -1);
        T34Delta guard2(mutableCounts(), pick.prev(), -1);
        T34Delta guard3(mutableCounts(), pick.next(), -1);
        (void) guard1; (void) guard2; (void) guard3;

        int seqStep = step(barkCt + 1);
        if (seqStep == 0)
            return false;
    }

    if (pick.val() <= 8 && mCounts[pick.next().id34()] != 0) {
        T34Delta guard1(mutableCounts(), pick, -1);
        T34Delta guard2(mutableCounts(), pick.next(), -1);
        (void) guard1; (void) guard2;

        int subSeqStep = step(barkCt + 1);
        if (subSeqStep == -1)
            return false;
    }

    if (pick.val() >= 2 && mCounts[pick.prev().id34()] != 0) {
        T34Delta guard1(mutableCounts(), pick, -1);
        T34Delta guard2(mutableCounts(), pick.prev(), -1);
        (void) guard1; (void) guard2;

        int subSeqStep = step(barkCt + 1);
        if (subSeqStep == -1)
            return false;
    }

    if (pick.val() <= 7 && mCounts[pick.nnext().id34()] != 0) {
        T34Delta guard1(mutableCounts(), pick, -1);
        T34Delta guard2(mutableCounts(), pick.nnext(), -1);
        (void) guard1; (void) guard2;

        int subSeqStep = step(barkCt + 1);
        if (subSeqStep == -1)
            return false;
    }

    if (pick.val() >= 3 && mCounts[pick.pprev().id34()] != 0) {
        T34Delta guard1(mutableCounts(), pick, -1);
        T34Delta guard2(mutableCounts(), pick.pprev(), -1);
        (void) guard1; (void) guard2;

        int subSeqStep = step(barkCt + 1);
        if (subSeqStep == -1)
            return false;
    }

    return true;
}

int TileCount::sum(const std::vector<T34> &ts) const
{
    int res = 0;
    for (T34 t : ts)
        res += mCounts[t.id34()];

    return res;
}

int TileCount::sum() const
{
    return std::accumulate(mCounts.begin(), mCounts.end(), 0);
}

std::array<int, 34> &TileCount::mutableCounts() const
{
    return const_cast<std::array<int, 34> &>(mCounts);
}

/// \brief cut-out meld and submeld from the count and get the max work-delta
/// \param i beginning tild id37
/// \param maxCut max number of meld to cut
/// \return step-4 delta
int TileCount::cutMeld(int id34, int maxCut) const
{
    if (maxCut == 0)
        return 0;

    while (id34 < 34 && mCounts[id34] == 0)
        id34++;

    if (id34 >= 34)
        return cutSubmeld(0, maxCut);

    int maxWork = 0;

    if (mCounts[id34] >= 3) {
        T34Delta guard(mutableCounts(), T34(id34), -3);
        (void) guard;
        int work = 2 + cutMeld(id34, maxCut - 1);
        maxWork = std::max(maxWork, work);
    }

    // is number && value 1~7 (index 0~6) && make a seq
    if (id34 + 2 < 27 && id34 % 9 <= 6 && mCounts[id34 + 1] > 0 && mCounts[id34 + 2] > 0) {
        T34Delta guard1(mutableCounts(), T34(id34), -1);
        T34Delta guard2(mutableCounts(), T34(id34 + 1), -1);
        T34Delta guard3(mutableCounts(), T34(id34 + 2), -1);
        (void) guard1; (void) guard2; (void) guard3;

        int work = 2 + cutMeld(id34, maxCut - 1);
        maxWork = std::max(maxWork, work);
    }

    int work = cutMeld(id34 + 1, maxCut);
    maxWork = std::max(maxWork, work);

    return maxWork;
}

std::vector<Parsed> TileCount::cutMeldOut(int id34, int maxCut) const
{
    std::vector<Parsed> reses;
    NonEmptyGuard guard(reses);
    (void) guard;

    while (id34 < 34 && mCounts[id34] == 0)
        id34++;

    if (id34 >= 34)
        return cutSubmeldOut(0, maxCut);

    const T34 t(id34);
    int maxWork = 0;

    // *INDENT-OFF*
    auto updateWork = [&](C34::Type type, bool cut) {
        auto subreses = cutMeldOut(id34 + !cut, maxCut - cut);
        int work = (cut ? 2 : 0) + subreses[0].work();

        if (work >= maxWork) {
            if (work > maxWork) {
                maxWork = work;
                reses.clear();
            }

            if (cut)
                for (Parsed &p : subreses)
                    p.append(C34(type, t));

            reses.insert(reses.end(), subreses.begin(), subreses.end());
        }
    };
    // *INDENT-ON*

    if (maxCut > 0) {
        if (ct(t) >= 3) {
            T34Delta guard(mutableCounts(), t, -3);
            (void) guard;
            updateWork(C34::Type::TRI, true);
        }

        if (t.isNum() && t.val() <= 7 && ct(t.next()) > 0 && ct(t.nnext()) > 0) {
            T34Delta guard1(mutableCounts(), t, -1);
            T34Delta guard2(mutableCounts(), t.next(), -1);
            T34Delta guard3(mutableCounts(), t.nnext(), -1);
            (void) guard1; (void) guard2; (void) guard3;
            updateWork(C34::Type::SEQ, true);
        }
    }

    updateWork(C34::Type::FREE, false);

    return reses;
}

/// \brief cut-out submeld from the count and get the max work-delta
/// \param i beginning tild id37
/// \param maxCut max number of submeld to cut
/// \return step-4 delta
int TileCount::cutSubmeld(int id34, int maxCut) const
{
    if (maxCut == 0)
        return 0;

    while (id34 < 34 && mCounts[id34] == 0)
        id34++;

    if (id34 >= 34)
        return 0;

    int maxWork = 0;

    if (mCounts[id34] >= 2) {
        T34Delta guard(mutableCounts(), T34(id34), -2);
        (void) guard;

        int work = 1 + cutSubmeld(id34, maxCut - 1);
        maxWork = std::max(maxWork, work);
    }

    // is number && value 1~8 (index 0~7) && neighbor
    if (id34 + 1 < 27 && id34 % 9 <= 7 && mCounts[id34 + 1] > 0) {
        T34Delta guard1(mutableCounts(), T34(id34), -1);
        T34Delta guard2(mutableCounts(), T34(id34 + 1), -1);
        (void) guard1; (void) guard2;

        int work = 1 + cutSubmeld(id34, maxCut - 1);
        maxWork = std::max(maxWork, work);
    }

    // is number && value 1~7 (index 0~6) && neighbor's neighbor
    if (id34 + 2 < 27 && id34 % 9 <= 6 && mCounts[id34 + 2] > 0) {
        T34Delta guard1(mutableCounts(), T34(id34), -1);
        T34Delta guard2(mutableCounts(), T34(id34 + 2), -1);
        (void) guard1; (void) guard2;

        int work = 1 + cutSubmeld(id34, maxCut - 1);
        maxWork = std::max(maxWork, work);
    }

    int work = cutSubmeld(id34 + 1, maxCut);
    maxWork = std::max(maxWork, work);

    return maxWork;
}

std::vector<Parsed> TileCount::cutSubmeldOut(int id34, int maxCut) const
{
    std::vector<Parsed> reses;
    NonEmptyGuard guard(reses);
    (void) guard;

    while (id34 < 34 && mCounts[id34] == 0)
        id34++;

    if (id34 >= 34)
        return reses;

    int maxWork = 0;
    const T34 t(id34);

    // *INDENT-OFF*
    auto updateWork = [&](C34::Type type, bool cut) {
        auto subreses = cutSubmeldOut(id34 + !cut, maxCut - cut);
        int work = cut + subreses[0].work();

        if (work >= maxWork) {
            if (work > maxWork) {
                maxWork = work;
                reses.clear();
            }

            for (Parsed &p : subreses)
                p.append(C34(type, t));

            reses.insert(reses.end(), subreses.begin(), subreses.end());
        }
    };
    // *INDENT-ON*

    if (maxCut > 0) {
        if (ct(t) >= 2) {
            T34Delta guard(mutableCounts(), t, -2);
            (void) guard;
            updateWork(C34::Type::PAIR, true);
        }

        if (t.isNum() && t.val() <= 8 && ct(t.next()) > 0) {
            T34Delta guard1(mutableCounts(), t, -1);
            T34Delta guard2(mutableCounts(), t.next(), -1);
            (void) guard1; (void) guard2;
            updateWork(t.val() == 1 || t.val() == 8 ? C34::Type::SIDE : C34::Type::BIFACE, true);
        }

        if (t.isNum() && t.val() <= 7 && ct(t.nnext()) > 0) {
            T34Delta guard1(mutableCounts(), T34(id34), -1);
            T34Delta guard2(mutableCounts(), T34(id34 + 2), -1);
            (void) guard1; (void) guard2;
            updateWork(C34::Type::CLAMP, true);
        }
    }

    updateWork(C34::Type::FREE, false);

    return reses;
}

bool TileCount::decomposeBirdless4(Explain4Closed &exp,
                                   const std::array<int, 34> &c) const
{
    std::array<int, 3> borrows { 0, 0, 0 };

    for (int tj = 0; tj < 34; tj++) {
        int remain = c[tj] - borrows[0];
        if (remain < 0) // reality conflicts imagination
            return false;

        if (remain >= 3) {
            exp.triplets.push_back(T34(tj));
            remain -= 3;
        }

        if (remain > 0) {
            if (T34(tj).isZ() || T34(tj).val() > 7)
                return false; // must be a floating tile

            borrows[1] += remain;
            borrows[2] += remain;
            while (remain-- > 0)
                exp.sequences.push_back(T34(tj));
        }

        borrows[0] = borrows[1];
        borrows[1] = borrows[2];
        borrows[2] = 0;
    }

    return true;
}



TileCount::T34Delta::T34Delta(std::array<int, 34> &c, T34 t, int delta)
    : mCount(c)
    , mTile(t)
    , mDelta(delta)
{
    assert(mCount[mTile.id34()] + mDelta >= 0);
    mCount[mTile.id34()] += mDelta;
}

TileCount::T34Delta::~T34Delta()
{
    mCount[mTile.id34()] -= mDelta;
}



TileCount::NonEmptyGuard::NonEmptyGuard(std::vector<Parsed> &p)
    : mParseds(p)
{
}

TileCount::NonEmptyGuard::~NonEmptyGuard()
{
    if (mParseds.empty())
        mParseds.push_back(Parsed());
}



} // namespace saki
