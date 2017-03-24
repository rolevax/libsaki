#include "hand.h"
#include "form.h"
#include "assume.h"
#include "util.h"
#include "action.h"

#include <array>
#include <algorithm>
#include <numeric>



namespace saki
{



HandDream HandDream::stay(const Hand &hand)
{
    if (hand.hasDrawn()) {
        return HandDream(Type::DRAWN_STAY, hand);
    } else {
        Type type = hand.closed().sum() % 3 == 2 ? Type::BARK_STAY : Type::PICK_STAY;
        return HandDream(type, hand);
    }
}

HandDream HandDream::spin(const Hand &hand)
{
    assert(hand.hasDrawn());
    return HandDream(Type::DRAWN_SPIN, hand);
}

HandDream HandDream::swap(const Hand &hand, const T37 &out)
{
    return HandDream(hand.hasDrawn() ? Type::DRAWN_SWAP : Type::BARK_OUT, hand, out);
}

HandDream HandDream::pick(const Hand &hand, const T37 &in)
{
    assert(!hand.hasDrawn());
    return HandDream(Type::PICK_IN, hand, in);
}

bool HandDream::ready() const
{
    TileCount &count = enter();
    bool res = false;

    if (step7() == 0 || step13() == 0) {
        res = true;
    } else if (step4() == 0) { // exclude 'pure empty' cases
        std::vector<T34> wait = effA();
        for (T34 t : wait) {
            if (count.ct(t) < 4) {
                res = true;
                break;// as long as exists one
            }
        }
    }

    leave();
    return res;
}

int HandDream::step() const
{
    TileCount &count = enter();
    int res = count.step(mHand.barks().size());
    leave();
    return res;
}

int HandDream::stepGb() const
{
    TileCount &count = enter();
    int res = count.stepGb(mHand.barks().size());
    leave();
    return res;
}

int HandDream::step4() const
{
    TileCount &count = enter();
    int res = count.step4(mHand.barks().size());
    leave();
    return res;
}

int HandDream::step7() const
{
    if (!mHand.isMenzen())
        return STEP_INF;

    TileCount &count = enter();
    int res = count.step7();
    leave();
    return res;
}

int HandDream::step7Gb() const
{
    if (!mHand.isMenzen())
        return STEP_INF;

    TileCount &count = enter();
    int res = count.step7Gb();
    leave();
    return res;
}

int HandDream::step13() const
{
    if (!mHand.isMenzen())
        return STEP_INF;

    TileCount &count = enter();
    int res = count.step13();
    leave();
    return res;
}

bool HandDream::hasEffA(T34 t) const
{
    TileCount &count = enter();

    int curr = step(); // should share a min step across 4, 7, 13
    HandDream dream(Type::PICK_IN, mHand, T37(t.id34()));
    bool res = (!count.dislike4(t) && dream.step4() < curr)
            || (mHand.isMenzen() && dream.step7() < curr)
            || (mHand.isMenzen() && t.isYao() && dream.step13() < curr);

    leave();
    return res;
}

bool HandDream::hasEffA4(T34 t) const
{
    TileCount &count = enter();
    HandDream dream(Type::PICK_IN, mHand, T37(t.id34()));
    bool res = !count.dislike4(t) && dream.step4() < step4();
    leave();
    return res;
}

bool HandDream::hasEffA7(T34 t) const
{
    enter();
    HandDream dream(Type::PICK_IN, mHand, T37(t.id34()));
    bool res = mHand.isMenzen() && dream.step7() < step7();
    leave();
    return res;
}

bool HandDream::hasEffA13(T34 t) const
{
    TileCount &count = enter();
    HandDream dream(Type::PICK_IN, mHand, T37(t.id34()));
    bool res = mHand.isMenzen() && t.isYao() && count.ct(t) < 2
            && dream.step13() < step13();
    leave();
    return res;
}

std::vector<T34> HandDream::effA() const
{
    std::vector<T34> res;

    for (int ti = 0; ti < 34; ti++) {
        T34 t(ti);
        if (hasEffA(t))
            res.push_back(t);
    }

    return res;
}

std::vector<T34> HandDream::effA4() const
{
    std::vector<T34> res;

    for (int ti = 0; ti < 34; ti++) {
        T34 t(ti);
        if (hasEffA4(t))
            res.push_back(t);
    }

    return res;
}

int HandDream::doraValueBy(const std::vector<T37> &inds) const
{
    assert(mDepth == 0);
    switch (mType) {
    case Type::DRAWN_STAY:
    case Type::PICK_STAY:
    case Type::BARK_STAY:
        return inds % mHand;
    case Type::DRAWN_SPIN:
        return (inds % mHand) - (inds % mHand.drawn());
    case Type::DRAWN_SWAP:
        return (inds % mHand) - (inds % mTile) + (inds % mHand.drawn());
    case Type::BARK_OUT:
        return (inds % mHand) - (inds % mTile);
    case Type::PICK_IN:
        return (inds % mHand) + (inds % mTile);
    default:
        unreached("");
    }
}

int HandDream::ctAka5() const
{
    assert(mDepth == 0);
    switch (mType) {
    case Type::DRAWN_STAY:
    case Type::PICK_STAY:
    case Type::BARK_STAY:
        return mHand.ctAka5();
    case Type::DRAWN_SPIN:
        return mHand.ctAka5() - mHand.drawn().isAka5();
    case Type::DRAWN_SWAP:
        return mHand.ctAka5() - mTile.isAka5() + mHand.drawn().isAka5();
    case Type::BARK_OUT:
        return mHand.ctAka5() - mTile.isAka5();
    case Type::PICK_IN:
        return mHand.ctAka5() + mTile.isAka5();
    default:
        unreached("");
    }
}

int HandDream::estimate(const RuleInfo &rule, int sw, int rw,
                        const std::vector<T37> &drids) const
{
    // rough estimation (theoritically not precise)
    // condition: menzen + dama + ron
    Hand hand(mHand); // use copy, cannot pass ill-state hand to form
    if (mType == Type::DRAWN_SPIN) {
        hand.spinOut();
    } else {
        assert(mType == Type::DRAWN_SWAP);
        hand.swapOut(mTile);
    }

    assert(hand.isMenzen() && hand.ready());

    PointInfo info;
    info.selfWind = sw;
    info.roundWind = rw;

    int max = 0;
    for (T34 t : hand.effA()) {
        T37 pick(t.id34());
        Form form(hand, pick, info, rule, drids);
        if (form.hasYaku())
            max = std::max(max, form.gain());
    }

    return max;
}

HandDream::HandDream(HandDream::Type type, const Hand &hand, const T37 &tile)
    : mType(type)
    , mHand(hand)
    , mTile(tile)
{
}

HandDream::HandDream(HandDream::Type type, const Hand &hand)
    : mType(type)
    , mHand(hand)
{
}

TileCount &HandDream::enter() const
{
    TileCount &count = mHand.mCount;

    if (mDepth == 0) {
        switch (mType) {
        case Type::DRAWN_STAY:
            count.inc(mHand.drawn(), 1);
            break;
        case Type::DRAWN_SWAP:
            count.inc(mTile, -1);
            count.inc(mHand.drawn(), 1);
            break;
        case Type::PICK_IN:
            count.inc(mTile, 1);
            break;
        case Type::BARK_OUT:
            count.inc(mTile, -1);
            break;
        case Type::DRAWN_SPIN:
        case Type::PICK_STAY:
        case Type::BARK_STAY:
            // do nothing
            break;
        }
    }

    mDepth++;

    return count;
}

void HandDream::leave() const
{
    TileCount &count = mHand.mCount;

    mDepth--;

    if (mDepth == 0) {
        switch (mType) {
        case Type::DRAWN_STAY:
            count.inc(mHand.drawn(), -1);
            break;
        case Type::DRAWN_SWAP:
            count.inc(mTile, 1);
            count.inc(mHand.drawn(), -1);
            break;
        case Type::PICK_IN:
            count.inc(mTile, -1);
            break;
        case Type::BARK_OUT:
            count.inc(mTile, 1);
            break;
        case Type::DRAWN_SPIN:
        case Type::PICK_STAY:
        case Type::BARK_STAY:
            // do nothing
            break;
        }
    }
}




Hand::Hand(const TileCount &count)
    : mCount(count)
{
    assert(mCount.sum() == 13);
}

Hand::Hand(const TileCount &count, const std::vector<M37> &barks)
    : mCount(count)
    , mBarks(barks)
{
}

const TileCount &Hand::closed() const
{
    return mCount;
}

const T37 &Hand::drawn() const
{
    assert(mHasDrawn);
    return mDrawn;
}


const std::vector<M37> &Hand::barks() const
{
    return mBarks;
}

bool Hand::hasDrawn() const
{
    return mHasDrawn;
}

bool Hand::isMenzen() const
{
    for (const M37 &m : mBarks)
        if (m.type() != M37::Type::ANKAN)
            return false;

    return true;
}

bool Hand::over4() const
{
    std::array<int, 34> sum;

    for (int ti = 0; ti < 34; ti++) {
        sum[ti] = mCount.ct(T34(ti));
        if (sum[ti] > 4)
            return true;
    }

    for (const M37 &m : mBarks) {
        for (const T37 &t : m.tiles()) {
            int id = t.id34();
            sum[id]++;
            if (sum[id] > 4)
                return true;
        }
    }

    return false;
}

bool Hand::nine9() const
{
    assert(mHasDrawn);

    int kind = 0;
    for (T34 t : tiles34::YAO13)
        kind += (mCount.ct(t) > 0 || t == mDrawn);

    return kind >= 9;
}

int Hand::ct(T34 t) const
{
    int res = 0;

    res += mHasDrawn && mDrawn == t;
    res += mCount.ct(t);
    for (const M37 &m : mBarks)
        res += std::count(m.tiles().begin(), m.tiles().end(), t);

    return res;
}

int Hand::ctAka5() const
{
    int inClosed = mCount.ctAka5();
    auto aka5InMeld = [](int s, const M37 &m) {
        const auto &ts = m.tiles();
        auto isAka5 = [](const T37 &t) { return t.isAka5(); };
        return s + std::count_if(ts.begin(), ts.end(), isAka5);
    };
    int inBarks = std::accumulate(mBarks.begin(), mBarks.end(), 0, aka5InMeld);
    int inDrawn = mHasDrawn ? mDrawn.isAka5() : 0;
    return inClosed + inBarks + inDrawn;
}

bool Hand::canChii(T34 t) const
{
    return canChiiAsLeft(t) || canChiiAsMiddle(t) || canChiiAsRight(t);
}

bool Hand::canChiiAsLeft(T34 t) const
{
    if (t.isZ() || t.val() >= 8)
        return false;

    if (!(mCount.ct(t.next()) > 0 && mCount.ct(t.nnext()) > 0))
        return false; // no material you eat a J8

    return hasSwappableAfterChii(t.next(), t.nnext(), [t](T34 out){
        return t != out && !(t ^ out);
    });
}

bool Hand::canChiiAsMiddle(T34 t) const
{
    if (t.isZ() || t.val() == 1 || t.val() == 9)
        return false;

    if (!(mCount.ct(t.prev()) > 0 && mCount.ct(t.next()) > 0))
        return false; // no material you eat a J8

    return hasSwappableAfterChii(t.prev(), t.next(), [t](T34 out){
        return t != out;
    });
}

bool Hand::canChiiAsRight(T34 t) const
{
    if (t.isZ() || t.val() <= 2)
        return false;

    if (!(mCount.ct(t.pprev()) > 0 && mCount.ct(t.prev()) > 0))
        return false; // no material you eat a J8

    return hasSwappableAfterChii(t.pprev(), t.prev(), [t](T34 out){
        return out != t && !(out ^ t);
    });
}

bool Hand::canPon(T34 t) const
{
    return mCount.ct(t) >= 2;
}

bool Hand::canDaiminkan(T34 t) const
{
    return mCount.ct(t) >= 3;
}

bool Hand::canAnkan(std::vector<T34> &choices, bool riichi) const
{
    assert(mHasDrawn);
    assert(choices.empty());

    for (int ti = 0; ti < 34; ti++) {
        T34 t(ti);
        switch (mCount.ct(t)) {
        case 3:
            if (t == mDrawn && (!riichi || mCount.onlyInTriplet(t, mBarks.size())))
                choices.emplace_back(t);
            break;
        case 4:
            if (!riichi)
                choices.emplace_back(t);
            break;
        default:
            break;
        }
    }

    return !choices.empty();
}

bool Hand::canKakan(std::vector<int> &barkIds) const
{
    assert(mHasDrawn);
    assert(barkIds.empty());

    for (size_t i = 0; i < mBarks.size(); i++) {
        if (mBarks[i].type() == M37::Type::PON
                && (mDrawn == mBarks[i][0] || mCount.ct(T34(mBarks[i][0])) == 1)) {
            barkIds.push_back(i);
        }
    }

    return !barkIds.empty();
}

bool Hand::canRon(T34 t, const PointInfo &info, const RuleInfo &rule, bool &doujun) const
{
    assert(!mHasDrawn);

    T37 pick(t.id34()); // whether aka5 does not affect ronablity
    if (withPick(pick).step() != -1)
        return false;

    bool yaku = Form(*this, pick, info, rule).hasYaku();
    doujun = !yaku;
    return yaku;
}

bool Hand::canTsumo(const PointInfo &info, const RuleInfo &rule) const
{
    assert(mHasDrawn);
    return step() == -1 && Form(*this, info, rule).hasYaku();
}

bool Hand::canRiichi() const
{
    assert(mHasDrawn);
    return isMenzen() && (ready() || step() == -1);
}

bool Hand::ready() const
{
    return HandDream::stay(*this).ready();
}

int Hand::step() const
{
    return HandDream::stay(*this).step();
}

int Hand::stepGb() const
{
    return HandDream::stay(*this).stepGb();
}

int Hand::step4() const
{
    return HandDream::stay(*this).step4();
}

int Hand::step7() const
{
    return HandDream::stay(*this).step7();
}

int Hand::step7Gb() const
{
    return HandDream::stay(*this).step7Gb();
}

int Hand::step13() const
{
    return HandDream::stay(*this).step13();
}

bool Hand::hasEffA(T34 t) const
{
    return HandDream::stay(*this).hasEffA(t);
}

bool Hand::hasEffA4(T34 t) const
{
    return HandDream::stay(*this).hasEffA4(t);
}

bool Hand::hasEffA7(T34 t) const
{
    return HandDream::stay(*this).hasEffA7(t);
}

bool Hand::hasEffA13(T34 t) const
{
    return HandDream::stay(*this).hasEffA13(t);
}

std::vector<T34> Hand::effA() const
{
    return HandDream::stay(*this).effA();
}

std::vector<T34> Hand::effA4() const
{
    return HandDream::stay(*this).effA4();
}

HandDream Hand::withAction(const Action &action) const
{
    switch (action.act()) {
    case ActCode::SWAP_OUT:
        return withSwap(action.tile());
    case ActCode::SPIN_OUT:
        return withSpin();
    default:
        unreached("Hand:withAction");
    }
}

HandDream Hand::withPick(const T37 &pick) const
{
    return HandDream::pick(*this, pick);
}

HandDream Hand::withSwap(const T37 &out) const
{
    return HandDream::swap(*this, out);
}

HandDream Hand::withSpin() const
{
    return HandDream::spin(*this);
}

void Hand::draw(const T37 &in)
{
    assert(!hasDrawn());
    mDrawn = in;
    mHasDrawn = true;
}

void Hand::swapOut(const T37 &out)
{
    assert(mCount.ct(out) > 0);
    mCount.inc(out, -1);
    if (mHasDrawn) {
        mCount.inc(mDrawn, 1);
        mHasDrawn = false;
    }
}

void Hand::spinOut()
{
    assert(mHasDrawn);
    mHasDrawn = false;
}

std::vector<T37> Hand::chiiAsLeft(const T37 &pick, bool showAka5)
{
    T37 m = tryShow(pick.next(), showAka5);
    T37 r = tryShow(pick.nnext(), showAka5);

    mBarks.push_back(M37::chii(pick, m, r, 0));

    return makeChoices([&pick](T34 t) { return pick != t && !(pick ^ t); });
}

std::vector<T37> Hand::chiiAsMiddle(const T37 &pick, bool showAka5)
{
    T37 l = tryShow(pick.prev(), showAka5);
    T37 r = tryShow(pick.next(), showAka5);

    mBarks.push_back(M37::chii(l, pick, r, 1));

    return makeChoices([&pick](T34 t) { return pick != t; });
}

std::vector<T37> Hand::chiiAsRight(const T37 &pick, bool showAka5)
{
    T37 l = tryShow(pick.pprev(), showAka5);
    T37 m = tryShow(pick.prev(), showAka5);

    mBarks.push_back(M37::chii(l, m, pick, 2));

    return makeChoices([&pick](T34 t) { return pick != t && !(t ^ pick); });
}

std::vector<T37> Hand::pon(const T37 &pick, int showAka5, int layIndex)
{
    assert(0 <= showAka5 && showAka5 <= 2);
    assume_opt_out(0 <= showAka5 && showAka5 <= 2);

    T37 one = tryShow(pick, showAka5 >= 1);
    T37 two = tryShow(pick, showAka5 == 2);

    if (layIndex == 0) {
        mBarks.push_back(M37::pon(pick, one, two, layIndex));
    } else if (layIndex == 1) {
        mBarks.push_back(M37::pon(one, pick, two, layIndex));
    } else if (layIndex == 2) {
        mBarks.push_back(M37::pon(one, two, pick, layIndex));
    } else {
        unreached("Hand::pon(): illigal lay index");
    }

    return makeChoices([&pick](T34 t) { return pick != t; });
}

void Hand::daiminkan(const T37 &pick, int layIndex)
{
    // 'showAka5' arg <- don't care
    T37 one = tryShow(pick, true);
    T37 two = tryShow(pick, true);
    T37 three = tryShow(pick, true);

    if (layIndex == 0) {
        mBarks.push_back(M37::daiminkan(pick, one, two, three, layIndex));
    } else if (layIndex == 1) {
        mBarks.push_back(M37::daiminkan(one, pick, two, three, layIndex));
    } else if (layIndex == 2) {
        mBarks.push_back(M37::daiminkan(one, two, pick, three, layIndex));
    } else {
        unreached("Hand::daiminkan(): illigal lay index");
    }
}

void Hand::ankan(T34 t)
{
    assert(mHasDrawn);
    bool useDrawn = t == mDrawn;

    // 'showAka5' arg <- don't care
    T37 one = tryShow(t, true);
    T37 two = tryShow(t, true);
    T37 three = tryShow(t, true);
    T37 four = useDrawn ? mDrawn : tryShow(t, true);

    if (!useDrawn)
        mCount.inc(mDrawn, 1);
    mHasDrawn = false;

    mBarks.push_back(M37::ankan(one, two, three, four));
}

void Hand::kakan(int barkId)
{
    assert(mHasDrawn);
    T37 t(mBarks[barkId][0]);
    if (t == mDrawn) {
        t = mDrawn;
    } else {
        if (mCount.ct(t) == 0)
            t = t.toInverse5();
        assert(mCount.ct(t) == 1);
        mCount.inc(t, -1);
        mCount.inc(mDrawn, 1);
    }

    mHasDrawn = false;

    mBarks[barkId].kakan(t);
}

void Hand::declareRiichi(std::vector<T37> &swappables, bool &spinnable) const
{
    assert(mHasDrawn);
    assert(util::all(mBarks, [](const M37 &m) { return m.type() == M37::Type::ANKAN; }));

    SwapOk ok = [this](T34 t) {
        if (mCount.ct(t) == 0)
            return false;
        T37 t37(t.id34());
        if (t37.val() == 5 && mCount.ct(t37) == 0)
            t37 = t37.toAka5();
        return withSwap(t37).ready();
    };
    swappables = makeChoices(ok);
    spinnable = withSpin().ready();
}

bool Hand::hasSwappableAfterChii(T34 mat1, T34 mat2, SwapOk ok) const
{
    for (int ti = 0; ti < 34; ti++) {
        T34 out(ti);
        int ct = mCount.ct(out);
        // storage >= 1
        // not banned as eat-swap
        // not the showing-out material, or else has at least two
        if (ct >= 1 && ok(out) && ((out != mat1 && out != mat2) || ct >= 2))
            return true;
    }

    return false;
}

T37 Hand::tryShow(T34 t34, bool showAka5)
{
    T37 t37(t34.id34());
    if (t37.val() == 5) {
        bool managed = showAka5 && mCount.ct(t37.toAka5()) > 0;
        bool passive = !showAka5 && mCount.ct(t37) == 0;
        if (managed || passive)
            t37 = t37.toAka5();
    }

    assert(mCount.ct(t37) > 0);
    mCount.inc(t37, -1);

    return t37;
}

std::vector<T37> Hand::makeChoices(SwapOk ok) const
{
    std::vector<T37> choices;
    choices.reserve(13); // no room for spin-out

    for (const T37 &t : tiles37::ORDER37)
        if (mCount.ct(t) > 0 && ok(t))
            choices.emplace_back(t);

    return choices;
}



int operator%(T34 ind, const Hand &hand)
{
    int inClosed = hand.closed().ct(ind.dora());

    const auto &barks = hand.barks();
    int inBarks = std::accumulate(barks.begin(), barks.end(), 0,
                                  [&ind](int s, const M37 &m) { return s + (ind % m); });

    int inDrawn = hand.hasDrawn() ? (ind % hand.drawn()) : 0;

    return inClosed + inBarks + inDrawn;
}

int operator%(const std::vector<T37> &inds, const Hand &hand)
{
    return std::accumulate(inds.begin(), inds.end(), 0,
                           [&hand](int s, const T37 &t) { return s + (t % hand); });
}

int operator%(const std::vector<T37> &inds, const HandDream &dream)
{
    return dream.doraValueBy(inds);
}



} // namespace saki


