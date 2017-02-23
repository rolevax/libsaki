#include "mount.h"
#include "util.h"

#include <numeric>
#include <algorithm>
#include <limits>
#include <cassert>



namespace saki
{



Exist::Exist()
{
    mBlack.fill(0);
    mRed.fill(0);
}

void Exist::inc(const T37 &t, int delta)
{
    if (t.isAka5())
        mRed[static_cast<int>(t.suit())] += delta;
    else
        mBlack[t.id34()] += delta;
}

void Exist::inc(T34 t, int delta)
{
    if (t.val() == 5) {
        int red = delta / 4;
        int black = delta - red;
        mRed[static_cast<int>(t.suit())] += red;
        mBlack[t.id34()] += black;
    } else {
        mBlack[t.id34()] += delta;
    }
}

void Exist::addBaseMk(const TileCount &stoch)
{
    for (int ti = 0; ti < 34; ti++) {
        T37 t(ti);
        inc(t, stoch.ct(t) * BASE_MK);
    }

    for (Suit s : { Suit::M, Suit::P, Suit::S }) {
        T37 t(s, 0);
        inc(t, stoch.ct(t) * BASE_MK);
    }

}

Exist::Polar Exist::polarize(const TileCount &stoch) const
{
    Polar res;

    auto add = [&res, &stoch](const T37 &t, int d) {
        if (stoch.ct(t) > 0)
            (res.*(d > 0 ? &Polar::pos : &Polar::npos)).emplace_back(t, d);
    };

    for (int ti = 0; ti < 34; ti++)
        add(T37(ti), mBlack[ti]);

    for (Suit s : { Suit::M, Suit::P, Suit::S })
        add(T37(s, 0), mRed[static_cast<int>(s)]);

    return res;
}



MountPrivate::MountPrivate(TileCount::AkadoraCount fillMode)
    : mStochA(fillMode)
    , mStochB() // empty
{
}



Mount::Mount(TileCount::AkadoraCount fillMode)
    : MountPrivate(fillMode)
{
}

Mount::Mount(const Mount &copy)
    : MountPrivate(copy)
{
    for (int i = 0; i < NUM_EXITS; i++) {
        const ErwinQueue &eq = copy.mErwinQueues[i];
        for (const std::unique_ptr<Erwin> &ptr : eq)
            mErwinQueues[i].emplace_back(ptr ? new Erwin(*ptr) : nullptr);
    }
}

Mount &Mount::operator=(const Mount &copy)
{
    MountPrivate::operator=(copy);

    for (int i = 0; i < NUM_EXITS; i++) {
        const ErwinQueue &eq = copy.mErwinQueues[i];
        mErwinQueues[i].clear();
        for (const std::unique_ptr<Erwin> &ptr : eq)
            mErwinQueues[i].emplace_back(ptr ? new Erwin(*ptr) : nullptr);
    }

    return *this;
}

void Mount::initFill(Rand &rand, TileCount &init, Exist &exist)
{
    int need = 13 - init.sum();

    assert(0 <= need && need <= 13);
    assert(wallRemain() >= 70 + need);
    mRemain -= need;

    std::vector<T37> pops = popExist(rand, exist, need);
    for (const T37 &t : pops)
        init.inc(t, 1);
}

const T37 &Mount::initPopExact(const T37 &t)
{
    mRemain--;
    mStochA.inc(t, -1);
    return t;
}

T37 Mount::wallPop(Rand &rand)
{
    assert(wallRemain() > 0);
    mRemain--;
    return popFrom(rand, Exit::WALL);
}

T37 Mount::deadPop(Rand &rand)
{
    assert(wallRemain() > 0);
    assert(deadRemain() > 0);
    mRemain--;
    mKanCt++;
    return popFrom(rand, Exit::DEAD);
}

int Mount::wallRemain() const
{
    return mRemain;
}

int Mount::deadRemain() const
{
    return 4 - mKanCt;
}

int Mount::remainA(T34 t) const
{
    return mStochA.ct(t);
}

int Mount::remainA(const T37 &t) const
{
    return mStochA.ct(t);
}

bool Mount::affordA(const TileCount &need) const
{
    for (const T37 &t : tiles37::ORDER37) {
        int demand = need.ct(t);
        int supply = mStochA.ct(t);
        if (demand > supply)
            return false;
    }

    return true;
}

const std::vector<T37> &Mount::getDrids() const
{
    return mDrids;
}

const std::vector<T37> &Mount::getUrids() const
{
    return mUrids;
}

void Mount::lightA(T34 t, int delta, bool rinshan)
{
    power(rinshan ? Exit::DEAD : Exit::WALL, 0, t, delta, false);
}

void Mount::lightA(const T37 &t, int delta, bool rinshan)
{
    power(rinshan ? Exit::DEAD : Exit::WALL, 0, t, delta, false);
}

void Mount::lightB(T34 t, int delta, bool rinshan)
{
    power(rinshan ? Exit::DEAD : Exit::WALL, 0, t, delta, true);
}

void Mount::lightB(const T37 &t, int delta, bool rinshan)
{
    power(rinshan ? Exit::DEAD : Exit::WALL, 0, t, delta, true);
}

void Mount::power(Exit exit, size_t pos, T34 t, int delta, bool bSpace)
{
    auto &ptr = prepareSuperpos(exit, pos);
    if (ptr->state == Erwin::SUPERPOS) {
        Exist &exist = *(bSpace ? ptr->exB : ptr->exA);
        exist.inc(t, delta);
    }
}

void Mount::power(Mount::Exit exit, size_t pos, const T37 &t, int delta, bool bSpace)
{
    auto &ptr = prepareSuperpos(exit, pos);
    if (ptr->state == Erwin::SUPERPOS) {
        Exist &exist = *(bSpace ? ptr->exB : ptr->exA);
        exist.inc(t, delta);
    }
}

void Mount::pin(Exit exit, std::size_t pos, const T37 &tile)
{
    ErwinQueue &eq = mErwinQueues[exit];

    while (!(pos < eq.size()))
        eq.emplace_back(nullptr);

    auto it = eq.begin();
    std::advance(it, pos);
    std::unique_ptr<Erwin> &ptr = *it;

    if (ptr == nullptr) {
        (mStochA.ct(tile) > 0 ? mStochA : mStochB).inc(tile, -1);
        ptr.reset(new Erwin(tile));
    } else {
        switch (ptr->state) {
        case Erwin::State::DEFINITE:
            // second pinning ignored
            break;
        case Erwin::State::SUPERPOS:
            // override stochastic chocolate
            (mStochA.ct(tile) > 0 ? mStochA : mStochB).inc(tile, -1);
            ptr.reset(new Erwin(tile));
            break;
        }
    }
}

void Mount::loadB(const T37 &t, int count)
{
    assert(count >= 0);

    // if cannot supply, just try best
    if (mStochA.ct(t) < count)
        count = mStochA.ct(t);

    mStochA.inc(t, -count);
    mStochB.inc(t, count);
}

void Mount::flipIndic(Rand &rand)
{
    mDrids.push_back(popFrom(rand, Exit::DORA));
}

void Mount::digIndic(Rand &rand)
{
    while (mUrids.size() < mDrids.size())
        mUrids.push_back(popFrom(rand, Exit::URADORA));
}

const std::unique_ptr<Mount::Erwin> &Mount::prepareSuperpos(Exit exit, std::size_t pos)
{
    ErwinQueue &eq = mErwinQueues[exit];

    while (!(pos < eq.size()))
        eq.emplace_back(nullptr);

    auto it = eq.begin();
    std::advance(it, pos);
    auto &ptr = *it;

    if (ptr == nullptr)
        ptr.reset(new Erwin());

    return ptr;
}

T37 Mount::popFrom(Rand &rand, Exit exit)
{
    ErwinQueue &eq = mErwinQueues[exit];
    if (eq.empty()) {
        return popScientific(rand);
    } else if (eq.front() == nullptr) {
        eq.pop_front();
        return popScientific(rand);
    } else {
        Erwin &e = *eq.front();
        T37 res;

        switch (e.state) {
        case Erwin::SUPERPOS:
            res = popExist(rand, *e.exA, *e.exB);
            break;
        case Erwin::DEFINITE:
            res = e.tile;
            break;
        }

        eq.pop_front();
        return res;
    }
}

std::vector<T37> Mount::popExist(Rand &rand, Exist &exist, int need)
{
    exist.addBaseMk(mStochA);
    Exist::Polar polar = exist.polarize(mStochA);
    return popPolar(rand, polar, mStochA, need);
}

T37 Mount::popExist(Rand &rand, Exist &exA, Exist &exB)
{
    exA.addBaseMk(mStochA);
    // no natrual existance for stoch-B

    Exist::Polar polarA = exA.polarize(mStochA);
    Exist::Polar polarB = exB.polarize(mStochB);

    using Cy = Exist::Polar::Cy;

    if (polarA.pos.empty() && polarB.pos.empty()) {
        auto less = [](Cy &l, Cy &r) { return l.e < r.e; };
        auto iterA = std::max_element(polarA.npos.begin(), polarA.npos.end(), less);
        auto iterB = std::max_element(polarB.npos.begin(), polarB.npos.end(), less);
        int maxA = iterA == polarA.npos.end() ? std::numeric_limits<int>::min() : iterA->e;
        int maxB = iterB == polarB.npos.end() ? std::numeric_limits<int>::min() : iterB->e;
        return maxA > maxB ? popPolar(rand, polarA, mStochA, 1).at(0)
                           : popPolar(rand, polarB, mStochB, 1).at(0);
    } else if (polarA.pos.empty()) {
        return popPolar(rand, polarB, mStochB, 1).at(0);
    } else if (polarB.pos.empty()) {
        return popPolar(rand, polarA, mStochA, 1).at(0);
    } else {
        auto plus = [](int s, Cy &p) { return s + p.e; };
        int sumA = std::accumulate(polarA.pos.begin(), polarA.pos.end(), 0, plus);
        int sumB = std::accumulate(polarB.pos.begin(), polarB.pos.end(), 0, plus);
        bool inA = rand.gen(sumA + sumB) < sumA;
        return inA ? popPolar(rand, polarA, mStochA, 1).at(0)
                   : popPolar(rand, polarB, mStochB, 1).at(0);
    }
}

std::vector<T37> Mount::popPolar(Rand &rand, Exist::Polar &polar, TileCount &stoch, int need)
{
    std::vector<T37> res;
    res.reserve(need);
    int sum = 0;

    if (!polar.pos.empty()) { // init pos-polar
        auto plus = [](int s, const Exist::Polar::Cy &p) { return s + p.e; };
        sum = std::accumulate(polar.pos.begin(), polar.pos.end(), 0, plus);
    }

    while (need --> 0) {
        T37 pop;
        int index;
        if (polar.pos.empty()) {
            assert(!polar.npos.empty());
            using Cy = Exist::Polar::Cy;
            auto less = [](const Cy &l, const Cy &r) { return l.e < r.e; };
            auto it = std::max_element(polar.npos.begin(), polar.npos.end(), less);
            index = it - polar.npos.begin();
            pop = polar.npos[index].t;
        } else {
            int r = rand.gen(sum);
            index = 0;
            while (!(r < polar.pos[index].e))
                r -= polar.pos[index++].e;
            pop = polar.pos[index].t;
        }

        stoch.inc(pop, -1);
        res.emplace_back(pop);

        if (need > 0) {
            if (stoch.ct(pop) == 0) { // last tile popped
                if (polar.pos.empty()) {
                    polar.npos[index].e = std::numeric_limits<int>::min();
                } else {
                    sum -= polar.pos[index].e;
                    polar.pos.erase(polar.pos.begin() + index);
                }
            } else if (!polar.pos.empty()) {
                // match the probablity
                sum -= Exist::BASE_MK;
                polar.pos[index].e -= Exist::BASE_MK;
            }
        }
    }

    return res;
}

T37 Mount::popScientific(Rand &rand)
{
    int sum = mStochA.sum();
    TileCount &stoch = sum > 0 ? mStochA : mStochB;
    if (sum == 0)
        sum = mStochB.sum();
    assert(sum > 0);

    int r = rand.gen(sum);
    int i34 = 0;
    while (!(r < stoch.ct(T34(i34))))
        r -= stoch.ct(T34(i34++));

    T37 ret(i34);

    if (ret.val() == 5) {
        int black = stoch.ct(ret);
        int red = stoch.ct(ret.toAka5());
        if (rand.gen(red + black) < red)
            ret = ret.toAka5();
    }

    stoch.inc(ret, -1);

    return ret;
}



} // namespace saki


