#include "mount.h"

#include <algorithm>
#include <numeric>
#include <limits>



namespace saki
{



Exist::Exist()
{
    mBlack.fill(0);
    mRed.fill(0);
}

void Exist::incMk(const T37 &t, int delta)
{
    if (t.isAka5())
        mRed[static_cast<int>(t.suit())] += delta;
    else
        mBlack[t.id34()] += delta;
}

void Exist::incMk(T34 t, int delta)
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
    for (T34 t : tiles34::ALL34)
        incMk(t, stoch.ct(T37(t.id34())) * BASE_MK);

    using namespace tiles37;
    for (const T37 &t : { 0_m, 0_p, 0_s })
        incMk(t, stoch.ct(t) * BASE_MK);
}

Exist::Polar Exist::polarize(const TileCount &stoch) const
{
    Polar res;

    // *INDENT-OFF*
    auto add = [&res, &stoch](const T37 &t, int d) {
        if (stoch.ct(t) > 0)
            (res.*(d > 0 ? &Polar::pos : &Polar::npos)).emplace_back(t, d);
    };
    // *INDENT-ON*

    for (int ti = 0; ti < 34; ti++)
        add(T37(ti), mBlack[ti]);

    for (Suit s : { Suit::M, Suit::P, Suit::S })
        add(T37(s, 0), mRed[static_cast<int>(s)]);

    return res;
}



Mount::Mount(TileCount::AkadoraCount fillMode)
    : mStockA(fillMode)
    , mStockB() // empty
{
}

void Mount::initFill(util::Rand &rand, TileCount &init, Exist &exist)
{
    int need = 13 - init.sum();

    assert(0 <= need && need <= 13);
    assert(remainPii() >= 70 + need);
    mRemain -= need;

    std::vector<T37> pops = popExist(rand, exist, need);
    for (const T37 &t : pops)
        init.inc(t, 1);
}

const T37 &Mount::initPopExact(const T37 &t)
{
    mRemain--;
    mStockA.inc(t, -1);
    return t;
}

T37 Mount::pop(util::Rand &rand, bool rinshan)
{
    assert(remainPii() > 0);
    mRemain--;

    if (rinshan) {
        assert(remainRinshan() > 0);
        mKanCt++;
    }

    return popFrom(rand, rinshan ? Exit::RINSHAN : Exit::PII);
}

int Mount::remainPii() const
{
    return mRemain;
}

int Mount::remainRinshan() const
{
    return 4 - mKanCt;
}

int Mount::remainA(T34 t) const
{
    return mStockA.ct(t);
}

int Mount::remainA(const T37 &t) const
{
    return mStockA.ct(t);
}

const util::Stactor<T37, 5> &Mount::getDrids() const
{
    return mDrids;
}

const util::Stactor<T37, 5> &Mount::getUrids() const
{
    return mUrids;
}

const TileCount &Mount::getStockA() const
{
    return mStockA;
}

void Mount::lightA(T34 t, int delta, bool rinshan)
{
    incMk(rinshan ? Exit::RINSHAN : Exit::PII, 0, t, delta, false);
}

void Mount::lightA(const T37 &t, int delta, bool rinshan)
{
    incMk(rinshan ? Exit::RINSHAN : Exit::PII, 0, t, delta, false);
}

void Mount::lightB(T34 t, int delta, bool rinshan)
{
    incMk(rinshan ? Exit::RINSHAN : Exit::PII, 0, t, delta, true);
}

void Mount::lightB(const T37 &t, int delta, bool rinshan)
{
    incMk(rinshan ? Exit::RINSHAN : Exit::PII, 0, t, delta, true);
}

void Mount::incMk(Exit exit, size_t pos, T34 t, int delta, bool bSpace)
{
    Erwin &erwin = prepareSuperpos(exit, pos);
    if (!erwin.earlyCollapse) {
        Exist &exist = bSpace ? erwin.exB : erwin.exA;
        exist.incMk(t, delta);
    }
}

void Mount::incMk(Mount::Exit exit, size_t pos, const T37 &t, int delta, bool bSpace)
{
    Erwin &erwin = prepareSuperpos(exit, pos);
    if (!erwin.earlyCollapse) {
        Exist &exist = bSpace ? erwin.exB : erwin.exA;
        exist.incMk(t, delta);
    }
}

void Mount::collapse(Exit exit, std::size_t pos, const T37 &tile)
{
    ErwinQueue &eq = mErwinQueues[exit];

    while (!(pos < eq.size()))
        eq.emplace_back(std::nullopt);

    auto it = eq.begin();
    std::advance(it, pos);
    std::optional<Erwin> &opt = *it;

    // first come first collapse
    if (!opt.has_value() || !opt->earlyCollapse) {
        (mStockA.ct(tile) > 0 ? mStockA : mStockB).inc(tile, -1);
        opt.emplace(Erwin(tile));
    }
}

void Mount::loadB(const T37 &t, int count)
{
    assert(count >= 0);

    // if cannot supply, just try best
    if (mStockA.ct(t) < count)
        count = mStockA.ct(t);

    mStockA.inc(t, -count);
    mStockB.inc(t, count);
}

void Mount::flipIndic(util::Rand &rand)
{
    mDrids.pushBack(popFrom(rand, Exit::DORAHYOU));
}

void Mount::digIndic(util::Rand &rand)
{
    while (mUrids.size() < mDrids.size())
        mUrids.pushBack(popFrom(rand, Exit::URAHYOU));
}

Mount::Erwin &Mount::prepareSuperpos(Exit exit, std::size_t pos)
{
    ErwinQueue &eq = mErwinQueues[exit];

    while (!(pos < eq.size()))
        eq.emplace_back(std::nullopt);

    auto it = eq.begin();
    std::advance(it, pos);
    auto &opt = *it;

    if (!opt.has_value())
        opt.emplace(Erwin());

    return *opt;
}

T37 Mount::popFrom(util::Rand &rand, Exit exit)
{
    ErwinQueue &eq = mErwinQueues[exit];
    if (eq.empty()) {
        return popScientific(rand);
    } else if (!eq.front().has_value()) {
        eq.pop_front();
        return popScientific(rand);
    } else {
        Erwin &e = *(eq.front());
        T37 res = e.earlyCollapse ? e.tile : popExist(rand, e.exA, e.exB);
        eq.pop_front();
        return res;
    }
}

std::vector<T37> Mount::popExist(util::Rand &rand, Exist &exist, int need)
{
    exist.addBaseMk(mStockA);
    Exist::Polar polar = exist.polarize(mStockA);
    return popPolar(rand, polar, mStockA, need);
}

T37 Mount::popExist(util::Rand &rand, Exist &exA, Exist &exB)
{
    exA.addBaseMk(mStockA);
    // no natrual existance for stoch-B

    Exist::Polar polarA = exA.polarize(mStockA);
    Exist::Polar polarB = exB.polarize(mStockB);

    using Cy = Exist::Polar::Cy;

    if (polarA.pos.empty() && polarB.pos.empty()) {
        auto less = [](Cy &l, Cy &r) { return l.e < r.e; };
        auto iterA = std::max_element(polarA.npos.begin(), polarA.npos.end(), less);
        auto iterB = std::max_element(polarB.npos.begin(), polarB.npos.end(), less);
        int maxA = iterA == polarA.npos.end() ? std::numeric_limits<int>::min() : iterA->e;
        int maxB = iterB == polarB.npos.end() ? std::numeric_limits<int>::min() : iterB->e;
        return maxA > maxB ? popPolar(rand, polarA, mStockA, 1).at(0)
                           : popPolar(rand, polarB, mStockB, 1).at(0);
    } else if (polarA.pos.empty()) {
        return popPolar(rand, polarB, mStockB, 1).at(0);
    } else if (polarB.pos.empty()) {
        return popPolar(rand, polarA, mStockA, 1).at(0);
    } else {
        auto plus = [](int s, Cy &p) { return s + p.e; };
        int sumA = std::accumulate(polarA.pos.begin(), polarA.pos.end(), 0, plus);
        int sumB = std::accumulate(polarB.pos.begin(), polarB.pos.end(), 0, plus);
        bool inA = rand.gen(sumA + sumB) < sumA;
        return inA ? popPolar(rand, polarA, mStockA, 1).at(0)
                   : popPolar(rand, polarB, mStockB, 1).at(0);
    }
}

std::vector<T37> Mount::popPolar(util::Rand &rand, Exist::Polar &polar,
                                 TileCount &stoch, int need)
{
    std::vector<T37> res;
    res.reserve(need);
    int sum = 0;

    if (!polar.pos.empty()) { // init pos-polar
        auto plus = [](int s, const Exist::Polar::Cy &p) { return s + p.e; };
        sum = std::accumulate(polar.pos.begin(), polar.pos.end(), 0, plus);
    }

    while (need-- > 0) {
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

T37 Mount::popScientific(util::Rand &rand)
{
    int sum = mStockA.sum();
    TileCount &stoch = sum > 0 ? mStockA : mStockB;
    if (sum == 0)
        sum = mStockB.sum();

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



int operator%(const util::Stactor<T37, 5> &indics, T34 t)
{
    auto aux = [t](int s, const T37 &id) { return s + id % t; };
    return std::accumulate(indics.begin(), indics.end(), 0, aux);
}



} // namespace saki
