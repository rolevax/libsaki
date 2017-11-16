#include "hand.h"
#include "../form/form.h"
#include "../util/assume.h"
#include "../util/misc.h"

#include <array>
#include <algorithm>
#include <numeric>



namespace saki
{



Hand::Hand(const TileCount &count)
    : mClosed(count)
{
    assert(mClosed.sum() == 13);
}

Hand::Hand(const TileCount &count, const util::Stactor<M37, 4> &barks)
    : mClosed(count)
    , mBarks(barks)
{
}

const TileCount &Hand::closed() const
{
    return mClosed;
}

const T37 &Hand::drawn() const
{
    assert(mHasDrawn);
    return mDrawn;
}

const T37 &Hand::outFor(const Action &action) const
{
    switch (action.act()) {
    case ActCode::SWAP_OUT:
    case ActCode::SWAP_RIICHI:
    case ActCode::CHII_AS_LEFT:
    case ActCode::CHII_AS_MIDDLE:
    case ActCode::CHII_AS_RIGHT:
    case ActCode::PON:
        return action.t37();
    case ActCode::SPIN_OUT:
    case ActCode::SPIN_RIICHI:
        return drawn();
    default:
        unreached("Hand::outFor");
    }
}


const util::Stactor<M37, 4> &Hand::barks() const
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

    for (T34 t : tiles34::ALL34) {
        sum[t.id34()] = mClosed.ct(t);
        if (sum[t.id34()] > 4)
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

    if (hasDrawn()) {
        sum[mDrawn.id34()]++;
        if (sum[mDrawn.id34()] > 4)
            return true;
    }

    return false;
}

bool Hand::nine9() const
{
    assert(mHasDrawn);

    int kind = 0;
    for (T34 t : tiles34::YAO13)
        kind += (mClosed.ct(t) > 0 || t == mDrawn);

    return kind >= 9;
}

int Hand::ct(T34 t) const
{
    int res = 0;

    res += mHasDrawn && mDrawn == t;
    res += mClosed.ct(t);
    for (const M37 &m : mBarks)
        res += std::count(m.tiles().begin(), m.tiles().end(), t);

    return res;
}

int Hand::ctAka5() const
{
    int inClosed = mClosed.ctAka5();
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

    if (!(mClosed.ct(t.next()) > 0 && mClosed.ct(t.nnext()) > 0))
        return false; // no material you eat a J8

    return hasSwappableAfterChii(t.next(), t.nnext(), [t](T34 out){
        return t != out && !(t ^ out);
    });
}

bool Hand::canChiiAsMiddle(T34 t) const
{
    if (t.isZ() || t.val() == 1 || t.val() == 9)
        return false;

    if (!(mClosed.ct(t.prev()) > 0 && mClosed.ct(t.next()) > 0))
        return false; // no material you eat a J8

    return hasSwappableAfterChii(t.prev(), t.next(), [t](T34 out){
        return t != out;
    });
}

bool Hand::canChiiAsRight(T34 t) const
{
    if (t.isZ() || t.val() <= 2)
        return false;

    if (!(mClosed.ct(t.pprev()) > 0 && mClosed.ct(t.prev()) > 0))
        return false; // no material you eat a J8

    return hasSwappableAfterChii(t.pprev(), t.prev(), [t](T34 out){
        return out != t && !(out ^ t);
    });
}

bool Hand::canPon(T34 t) const
{
    return mClosed.ct(t) >= 2;
}

bool Hand::canCp(T34 pick, const Action &action) const
{
    using AC = ActCode;

    const T37 &out = action.t37();
    if (out == pick)
        return false;

    bool kuikae = false;
    bool needShow = false;
    bool tension = shouldShowAka5(out, action.showAka5() >= 1) == out.isAka5();

    switch (action.act()) {
    case AC::CHII_AS_LEFT:
        if (!canChiiAsLeft(pick))
            return false;
        kuikae = (pick ^ out);
        needShow = ((pick | out) || (pick || out)) && tension;
        break;
    case AC::CHII_AS_MIDDLE:
        if (!canChiiAsMiddle(pick))
            return false;
        needShow = ((out | pick) || (pick | out)) && tension;
        break;
    case AC::CHII_AS_RIGHT:
        if (!canChiiAsRight(pick))
            return false;
        kuikae = (out ^ pick);
        needShow = ((out || pick) || (out | pick)) && tension;
        break;
    case AC::PON:
        if (!canPon(pick))
            return false;
        break;
    default:
        unreached("Hand::canCp");
    }

    return !kuikae && mClosed.ct(out) >= (needShow ? 2 : 1);
}

bool Hand::canDaiminkan(T34 t) const
{
    return mClosed.ct(t) >= 3;
}

bool Hand::canAnkan(util::Stactor<T34, 3> &choices, bool riichi) const
{
    assert(mHasDrawn);
    assert(choices.empty());

    for (T34 t : tiles34::ALL34) {
        switch (mClosed.ct(t)) {
        case 3:
            if (t == mDrawn && (!riichi || mClosed.onlyInTriplet(t, mBarks.size())))
                choices.pushBack(t);
            break;
        case 4:
            if (!riichi)
                choices.pushBack(t);
            break;
        default:
            break;
        }
    }

    return !choices.empty();
}

bool Hand::canKakan(util::Stactor<int, 3> &barkIds) const
{
    assert(mHasDrawn);
    assert(barkIds.empty());

    for (size_t i = 0; i < mBarks.size(); i++) {
        if (mBarks[i].type() == M37::Type::PON
                && (mDrawn == mBarks[i][0] || mClosed.ct(T34(mBarks[i][0])) == 1)) {
            barkIds.pushBack(i);
        }
    }

    return !barkIds.empty();
}

bool Hand::canRon(T34 t, const FormCtx &ctx, const Rule &rule, bool &doujun) const
{
    assert(!mHasDrawn);

    if (mClosed.peekDraw(t, &TileCount::step, static_cast<int>(mBarks.size())) != -1)
        return false;

    T37 pick(t.id34()); // whether aka5 does not affect ronnablity
    bool yaku = Form(*this, pick, ctx, rule).hasYaku();
    doujun = !yaku;
    return yaku;
}

bool Hand::canTsumo(const FormCtx &ctx, const Rule &rule) const
{
    assert(mHasDrawn);
    return step() == -1 && Form(*this, ctx, rule).hasYaku();
}

bool Hand::canRiichi(util::Stactor<T37, 13> &swappables, bool &spinnable) const
{
    assert(mHasDrawn);

    if (!isMenzen())
        return false;

    SwapOk ok = [this](T34 t) {
        if (mClosed.ct(t) == 0)
            return false;
        T37 t37(t.id34());
        if (t37.val() == 5 && mClosed.ct(t37) == 0)
            t37 = t37.toAka5();
        return peekSwap(t37, &Hand::ready);
    };
    swappables = makeChoices(ok);
    spinnable = peekSpin(&Hand::ready);

    return !swappables.empty() || spinnable;
}

bool Hand::ready() const
{
    return step7() == 0 || step13() == 0
            || (step4() == 0 && util::any(effA(), [this](T34 t) { return ct(t) < 4; }));
}

int Hand::step() const
{
    return peekStay(&TileCount::step, mBarks.size());
}

int Hand::stepGb() const
{
    return peekStay(&TileCount::stepGb, mBarks.size());
}

int Hand::step4() const
{
    return peekStay(&TileCount::step4, mBarks.size());
}

int Hand::step7() const
{
    return mBarks.empty() ? peekStay(&TileCount::step7) : STEP_INF;
}

int Hand::step7Gb() const
{
    return mBarks.empty() ? peekStay(&TileCount::step7Gb) : STEP_INF;
}

int Hand::step13() const
{
    return mBarks.empty() ? peekStay(&TileCount::step13) : STEP_INF;
}

bool Hand::hasEffA(T34 t) const
{
    return peekStay(&TileCount::hasEffA, mBarks.size(), t);
}

bool Hand::hasEffA4(T34 t) const
{
    return peekStay(&TileCount::hasEffA4, mBarks.size(), t);
}

bool Hand::hasEffA7(T34 t) const
{
    return mBarks.empty() && peekStay(&TileCount::hasEffA7, t);
}

bool Hand::hasEffA13(T34 t) const
{
    return mBarks.empty() && peekStay(&TileCount::hasEffA13, t);
}

util::Stactor<T34, 34> Hand::effA() const
{
    return peekStay(&TileCount::effA, mBarks.size());
}

util::Stactor<T34, 34> Hand::effA4() const
{
    return peekStay(&TileCount::effA4, mBarks.size());
}

std::vector<Parsed> Hand::parse4() const
{
    return peekStay(&TileCount::parse4, mBarks.size());
}

// rough estimation (theoritically not precise)
// condition: menzen + dama + ron
int Hand::estimate(const Rule &rule, int sw, int rw, const util::Stactor<T37, 5> &drids) const
{
    assert(isMenzen() && ready());

    FormCtx ctx;
    ctx.selfWind = sw;
    ctx.roundWind = rw;

    int max = 0;
    for (T34 t : effA()) {
        T37 pick(t.id34());
        Form form(*this, pick, ctx, rule, drids);
        if (form.hasYaku())
            max = std::max(max, form.gain());
    }

    return max;
}

int Hand::peekPickStep4(T34 pick) const
{
    return mClosed.peekDraw(pick, &TileCount::step4, static_cast<int>(mBarks.size()));
}

int Hand::peekPickStep7(T34 pick) const
{
    return mBarks.empty() ? mClosed.peekDraw(pick, &TileCount::step7) : STEP_INF;
}

int Hand::peekPickStep7Gb(T34 pick) const
{
    return mBarks.empty() ? mClosed.peekDraw(pick, &TileCount::step7Gb) : STEP_INF;
}

int Hand::peekPickStep13(T34 pick) const
{
    return mBarks.empty() ? mClosed.peekDraw(pick, &TileCount::step13) : STEP_INF;
}

void Hand::draw(const T37 &in)
{
    assert(!hasDrawn());
    mDrawn = in;
    mHasDrawn = true;
}

void Hand::swapOut(const T37 &out)
{
    assert(mClosed.ct(out) > 0);
    assert(mHasDrawn);
    mClosed.inc(out, -1);
    mClosed.inc(mDrawn, 1);
    mHasDrawn = false;
}

void Hand::spinOut()
{
    assert(mHasDrawn);
    mHasDrawn = false;
}

void Hand::barkOut(const T37 &out)
{
    assert(mClosed.ct(out) > 0);
    assert(!mHasDrawn);
    mClosed.inc(out, -1);
}

void Hand::chiiAsLeft(const T37 &pick, bool showAka5)
{
    T37 m = tryShow(pick.next(), showAka5);
    T37 r = tryShow(pick.nnext(), showAka5);
    mBarks.pushBack(M37::chii(pick, m, r, 0));
}

void Hand::chiiAsMiddle(const T37 &pick, bool showAka5)
{
    T37 l = tryShow(pick.prev(), showAka5);
    T37 r = tryShow(pick.next(), showAka5);
    mBarks.pushBack(M37::chii(l, pick, r, 1));
}

void Hand::chiiAsRight(const T37 &pick, bool showAka5)
{
    T37 l = tryShow(pick.pprev(), showAka5);
    T37 m = tryShow(pick.prev(), showAka5);
    mBarks.pushBack(M37::chii(l, m, pick, 2));
}

void Hand::pon(const T37 &pick, int showAka5, int layIndex)
{
    assert(0 <= showAka5 && showAka5 <= 2);
    assume_opt_out(0 <= showAka5 && showAka5 <= 2);

    T37 one = tryShow(pick, showAka5 >= 1);
    T37 two = tryShow(pick, showAka5 == 2);

    if (layIndex == 0) {
        mBarks.pushBack(M37::pon(pick, one, two, layIndex));
    } else if (layIndex == 1) {
        mBarks.pushBack(M37::pon(one, pick, two, layIndex));
    } else if (layIndex == 2) {
        mBarks.pushBack(M37::pon(one, two, pick, layIndex));
    } else {
        unreached("Hand::pon(): illigal lay index");
    }
}

void Hand::daiminkan(const T37 &pick, int layIndex)
{
    // 'showAka5' arg <- don't care
    T37 one = tryShow(pick, true);
    T37 two = tryShow(pick, true);
    T37 three = tryShow(pick, true);

    if (layIndex == 0) {
        mBarks.pushBack(M37::daiminkan(pick, one, two, three, layIndex));
    } else if (layIndex == 1) {
        mBarks.pushBack(M37::daiminkan(one, pick, two, three, layIndex));
    } else if (layIndex == 2) {
        mBarks.pushBack(M37::daiminkan(one, two, pick, three, layIndex));
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
        mClosed.inc(mDrawn, 1);
    mHasDrawn = false;

    mBarks.pushBack(M37::ankan(one, two, three, four));
}

void Hand::kakan(int barkId)
{
    assert(mHasDrawn);
    T37 t(mBarks[barkId][0]);
    if (t == mDrawn) {
        t = mDrawn;
    } else {
        if (mClosed.ct(t) == 0)
            t = t.toInverse5();
        assert(mClosed.ct(t) == 1);
        mClosed.inc(t, -1);
        mClosed.inc(mDrawn, 1);
    }

    mHasDrawn = false;

    mBarks[barkId].kakan(t);
}

bool Hand::hasSwappableAfterChii(T34 mat1, T34 mat2, SwapOk ok) const
{
    for (T34 out : tiles34::ALL34) {
        int ct = mClosed.ct(out);
        // storage >= 1
        // not banned as eat-swap
        // not the showing-out material, or else has at least two
        if (ct >= 1 && ok(out) && ((out != mat1 && out != mat2) || ct >= 2))
            return true;
    }

    return false;
}

bool Hand::shouldShowAka5(T34 show, bool showAka5) const
{
    if (show.val() == 5) {
        T37 black(show.id34());
        bool managed = showAka5 && mClosed.ct(black.toAka5()) > 0;
        bool passive = !showAka5 && mClosed.ct(black) == 0;
        return managed || passive;
    }

    return false;
}

T37 Hand::tryShow(T34 t34, bool showAka5)
{
    T37 t37(t34.id34());
    if (shouldShowAka5(t34, showAka5))
        t37 = t37.toAka5();

    assert(mClosed.ct(t37) > 0);
    mClosed.inc(t37, -1);

    return t37;
}

util::Stactor<T37, 13> Hand::makeChoices(SwapOk ok) const
{
    util::Stactor<T37, 13> choices;

    for (const T37 &t : tiles37::ORDER37)
        if (mClosed.ct(t) > 0 && ok(t))
            choices.pushBack(t);

    return choices;
}



Hand::DeltaSpin::DeltaSpin(Hand &hand)
    : mHand(hand)
{
    mHand.spinOut();
}

Hand::DeltaSpin::~DeltaSpin()
{
    mHand.mHasDrawn = true;
}



Hand::DeltaSwap::DeltaSwap(Hand &hand, const T37 &out)
    : mHand(hand)
    , mOut(out)
{
    mHand.swapOut(out);
}

Hand::DeltaSwap::~DeltaSwap()
{
    mHand.mHasDrawn = true;
    mHand.mClosed.inc(mHand.mDrawn, -1);
    mHand.mClosed.inc(mOut, 1);
}

Hand::DeltaCp::DeltaCp(Hand &hand, const T37 &pick, const Action &a, const T37 &out)
    : mHand(hand)
    , mOut(out)
{
    switch (a.act()) {
    case ActCode::CHII_AS_LEFT:
        mHand.chiiAsLeft(pick, a.showAka5());
        break;
    case ActCode::CHII_AS_MIDDLE:
        mHand.chiiAsMiddle(pick, a.showAka5());
        break;
    case ActCode::CHII_AS_RIGHT:
        mHand.chiiAsRight(pick, a.showAka5());
        break;
    case ActCode::PON:
        mHand.pon(pick, a.showAka5(), 0);
        break;
    default:
        unreached("Hand::DeltaCp::ctor");
    }

    mHand.barkOut(a.t37());
}

Hand::DeltaCp::~DeltaCp()
{
    mHand.mClosed.inc(mOut, 1);

    const M37 &cp = mHand.mBarks.back();
    for (int i = 0; static_cast<size_t>(i) < cp.tiles().size(); i++)
        if (i != cp.layIndex())
            mHand.mClosed.inc(cp[i], 1);

    mHand.mBarks.popBack();
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

int operator%(const util::Stactor<T37, 5> &inds, const Hand &hand)
{
    return std::accumulate(inds.begin(), inds.end(), 0,
                           [&hand](int s, const T37 &t) { return s + (t % hand); });
}



} // namespace saki


