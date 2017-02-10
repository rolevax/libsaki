#include "tilecount.h"
#include "tile.h"

#include <numeric>
#include <algorithm>
#include <cassert>
#include <iostream>



namespace saki
{



TileCount::TileCount()
{
    c.fill(0);
    mAka5s.fill(0);
}

TileCount::TileCount(TileCount::AkadoraCount fillMode)
{
    c.fill(4);
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
    c.fill(0);
    mAka5s.fill(0);

    for (const T37 &t : t37s)
        inc(t, 1);
}

TileCount::TileCount(std::vector<T37> t37s)
{
    c.fill(0);
    mAka5s.fill(0);

    for (const T37 &t : t37s)
        inc(t, 1);
}

int TileCount::ct(T34 t) const
{
    return c[t.id34()];
}

int TileCount::ct(const T37 &t) const
{
    if (t.val() == 5) {
        int red = mAka5s[static_cast<int>(t.suit())];
        return t.isAka5() ? red : c[t.id34()] - red;
    }

    return c[t.id34()];
}

int TileCount::ct(const std::vector<T34> &ts) const
{
    return std::accumulate(ts.begin(), ts.end(), 0,
                           [this](int s, T34 t) { return s + ct(t); });
}

int TileCount::ct(const std::vector<T37> &ts) const
{
    return std::accumulate(ts.begin(), ts.end(), 0,
                           [this](int s, const T37 &t) { return s + ct(t); });
}

int TileCount::ctAka5() const
{
    return std::accumulate(mAka5s.begin(), mAka5s.end(), 0);
}

int TileCount::ctZ() const
{
    return std::accumulate(c.begin() + 27, c.end(), 0);
}

int TileCount::ctYao() const
{
    using namespace tiles34;
    return ct(1_m) + ct(9_m) + ct(1_p) + ct(9_p) + ct(1_s) + ct(9_s) + ctZ();
}

bool TileCount::hasZ() const
{
    return std::find_if(c.begin() + 27, c.end(), [](int i) { return i > 0; }) != c.end();
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

    c[t.id34()] += delta;
    if (t.isAka5())
        mAka5s[static_cast<int>(t.suit())] += delta;
}

TileCount &TileCount::operator-=(const TileCount &rhs)
{
    for (int ti = 0; ti < 34; ti++) {
        c[ti] -= rhs.c[ti];
        assert(c[ti] >= 0);
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

int TileCount::step4(int barkCt) const
{
    int maxCut = 4 - barkCt;
    int min = 8;

    // having-birdhead case
    for (int ti = 0; ti < 34; ti++) {
        if (c[ti] >= 2) {
            c[ti] -= 2;
            int comin = 7 - cutMeld(0, maxCut) - 2 * barkCt;
            min = std::min(min, comin);
            c[ti] += 2;
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
        if (c[ti] > 0) {
            needKind--;
            if (c[ti] >= 2)
                pair++;
        }
    }

    if (needKind <= 0)
        return 6 - pair;
    else
        return (6 - pair) + needKind;
}

int TileCount::step13() const
{
    int res = 13;
    bool gotPair = false;

    for (T34 t : tiles34::YAO13) {
        if (c[t.id34()] > 0) {
            res--;
            if (!gotPair && c[t.id34()] >= 2) {
                gotPair = true;
                res--;
            }
        }
    }

    return res;
}

std::vector<T34> TileCount::t34s() const
{
    std::vector<T34> res;

    for (int ti = 0; ti < 34; ti++)
        if (c[ti] > 0)
            res.emplace_back(ti);

    return res;
}

std::vector<T37> TileCount::t37s(bool allowDup) const
{
    std::vector<T37> res;

    for (const T37 &t : tiles37::ORDER37) {
        int num = ct(t);
        if (allowDup) {
            while (num --> 0)
                res.emplace_back(t);
        } else {
            if (num != 0)
                res.emplace_back(t);
        }
    }

    return res;
}

/*
std::vector<T34> TileCount::effB(int barkCt, const TileCount &remain) const
{
    std::vector<T34> res;

    int currEffA = remain.sum(effA(barkCt));
    int currStep = step4(barkCt);

    for (int ti = 0; ti < 34; ti++) {
        T34 t(ti);

        if (remain.ct(t) > 0) {
            c[t.id34()]++;

            // remain - 1 since one is drawn
            if (step(barkCt) == currStep
                    && remain.sum(effA(barkCt)) - 1 > currEffA)
                res.push_back(t);

            c[t.id34()]--;
        }
    }

    return res;
}
*/

// 't' will be a floating tile (form-4) if added to this 'count'
// a disliked tile might be 'useful' when there is a pure-empty ready
// but logically that's still not 'effective-A'
// since it does not change the step
bool TileCount::dislike4(T34 t) const
{
    int ti = t.id34();

    if (c[ti] > 0) // can make a pair
        return false;

    if (t.isNum()) {
        if (t.val() >= 2 && c[ti - 1] > 0)
            return false; // has left neighbor
        if (t.val() <= 8 && c[ti + 1] > 0)
            return false; // has right neighbor
        if (t.val() >= 3 && c[ti - 2] > 0)
            return false; // has left-left neighbor
        if (t.val() <= 7 && c[ti + 2] > 0)
            return false; // has right-right neighbor
    }

    return true; // nobody likes this tile
}

std::vector<TileCount::Explain4Closed> TileCount::explain4(T34 pick) const
{
    // no assertion. the result will be illegal if the input is illegal

    c[pick.id34()]++;

    std::vector<Explain4Closed> res;

    // enumerate for all possible birdheads
    for (int ti = 0; ti < 34; ti++) {
        if (c[ti] >= 2) {
            c[ti] -= 2;

            T34 pick(ti);
            Explain4Closed exp(pick);
            if (decomposeBirdless4(exp, c)) {
                res.push_back(exp);

                // 111-222-333 --> 123-123-123
                auto checkSequences = [](T34 l, T34 m, T34 r) -> bool {
                    return (l | m) && (m | r);
                };
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

            c[ti] += 2;
        }
    }

    c[pick.id34()]--;

    assert(!res.empty());
    return res;
}

bool TileCount::onlyInTriplet(T34 pick, int barkCt) const
{
    assert(step(barkCt) == 0); // this algo does not work when step is -1
    assert(c[pick.id34()] == 3);

    if (pick.isZ())
        return true;

    auto mod3 = [this](T34 x, T34 y, T34 z, int delta) {
        c[x.id34()] += delta;
        c[y.id34()] += delta;
        c[z.id34()] += delta;
    };

    auto mod2 = [this](T34 a, T34 b, int delta) {
        c[a.id34()] += delta;
        c[b.id34()] += delta;
    };

    if (pick.val() <= 7
            && c[pick.next().id34()] != 0
            && c[pick.nnext().id34()] != 0) {
        mod3(pick, pick.next(), pick.nnext(), -1);
        int seqStep = step(barkCt + 1);
        mod3(pick, pick.next(), pick.nnext(), 1);
        if (seqStep == 0)
            return false;
    }

    if (pick.val() >= 3
            && c[pick.prev().id34()] != 0
            && c[pick.pprev().id34()] != 0) {
        mod3(pick, pick.prev(), pick.pprev(), -1);
        int seqStep = step(barkCt + 1);
        mod3(pick, pick.prev(), pick.pprev(), 1);
        if (seqStep == 0)
            return false;
    }

    if (2 <= pick.val() && pick.val() <= 8
            && c[pick.prev().id34()] != 0
            && c[pick.next().id34()] != 0) {
        mod3(pick, pick.prev(), pick.next(), -1);
        int seqStep = step(barkCt + 1);
        mod3(pick, pick.prev(), pick.next(), 1);
        if (seqStep == 0)
            return false;
    }

    if (pick.val() <= 8 && c[pick.next().id34()] != 0) {
        mod2(pick, pick.next(), -1);
        int subSeqStep = step(barkCt + 1);
        mod2(pick, pick.next(), 1);
        if (subSeqStep == -1)
            return false;
    }

    if (pick.val() >= 2 && c[pick.prev().id34()] != 0) {
        mod2(pick, pick.prev(), -1);
        int subSeqStep = step(barkCt + 1);
        mod2(pick, pick.prev(), 1);
        if (subSeqStep == -1)
            return false;
    }

    if (pick.val() <= 7 && c[pick.nnext().id34()] != 0) {
        mod2(pick, pick.nnext(), -1);
        int subSeqStep = step(barkCt + 1);
        mod2(pick, pick.nnext(), 1);
        if (subSeqStep == -1)
            return false;
    }

    if (pick.val() >= 3 && c[pick.pprev().id34()] != 0) {
        mod2(pick, pick.pprev(), -1);
        int subSeqStep = step(barkCt + 1);
        mod2(pick, pick.pprev(), 1);
        if (subSeqStep == -1)
            return false;
    }

    return true;
}

int TileCount::sum(const std::vector<T34> &ts) const
{
    int res = 0;
    for (T34 t : ts)
        res += c[t.id34()];
    return res;
}

int TileCount::sum() const
{
    return std::accumulate(c.begin(), c.end(), 0);
}

int TileCount::cutMeld(int id34, int maxCut) const
{
    if (maxCut == 0)
        return 0;

    while (id34 < 34 && c[id34] == 0)
        id34++;

    if (id34 >= 34)
        return cutSubmeld(0, maxCut);

    int maxWork = 0;

    if (c[id34] >= 3) {
        c[id34] -= 3;
        int work = 2 + cutMeld(id34, maxCut - 1);
        c[id34] += 3;

        maxWork = std::max(maxWork, work);
    }

    // is number && value 1~7 (index 0~6) && make a seq
    if (id34 + 2 < 27 && id34 % 9 <= 6 && c[id34 + 1] > 0  && c[id34 + 2] > 0) {
        c[id34]--;
        c[id34 + 1]--;
        c[id34 + 2]--;
        int work = 2 + cutMeld(id34, maxCut - 1);
        c[id34]++;
        c[id34 + 1]++;
        c[id34 + 2]++;

        maxWork = std::max(maxWork, work);
    }

    int work = cutMeld(id34 + 1, maxCut);
    maxWork = std::max(maxWork, work);

    return maxWork;
}

int TileCount::cutSubmeld(int id34, int maxCut) const
{
    if (maxCut == 0)
        return 0;

    while (id34 < 34 && c[id34] == 0)
        id34++;

    if (id34 >= 34)
        return 0;

    int maxWork = 0;

    if (c[id34] >= 2) {
        c[id34] -= 2;
        int work = 1 + cutSubmeld(id34, maxCut - 1);
        c[id34] += 2;

        maxWork = std::max(maxWork, work);
    }

    // is number && value 1~8 (index 0~7) && neighbor
    if (id34 + 1 < 27 && id34 % 9 <= 7 && c[id34 + 1] > 0) {
        c[id34]--;
        c[id34 + 1]--;
        int work = 1 + cutSubmeld(id34, maxCut - 1);
        c[id34]++;
        c[id34 + 1]++;

        maxWork = std::max(maxWork, work);
    }

    // is number && value 1~7 (index 0~6) && neighbor's neighbor
    if (id34 + 2 < 27 && id34 % 9 <= 6 && c[id34 + 2] > 0) {
        c[id34]--;
        c[id34 + 2]--;
        int work = 1 + cutSubmeld(id34, maxCut - 1);
        c[id34]++;
        c[id34 + 2]++;

        maxWork = std::max(maxWork, work);
    }

    int work = cutSubmeld(id34 + 1, maxCut);
    maxWork = std::max(maxWork, work);

    return maxWork;
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
            while (remain --> 0)
                exp.sequences.push_back(T34(tj));
        }

        borrows[0] = borrows[1];
        borrows[1] = borrows[2];
        borrows[2] = 0;
    }

    return true;
}



} // namespace saki


