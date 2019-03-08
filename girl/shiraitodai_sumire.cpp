#include "shiraitodai_sumire.h"
#include "../table/princess.h"
#include "../table/table.h"
#include "../table/table_view_hand.h"
#include "../ai/ai.h"
#include "../util/misc.h"

#include <sstream>



namespace saki
{



void Sumire::onDice(util::Rand &rand, const Table &table)
{
    (void) rand;
    (void) table;

    mTarget = Who();
    mShootTrial = 0;
}

bool Sumire::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    if (who != mSelf || iter > 100)
        return true;

    int sw = table.getSelfWind(mSelf);
    int rw = table.getRoundWind();
    if (iter < 5)
        for (T34 z : tiles34::Z7)
            if (z.isYakuhai(sw, rw) && init.closed().ct(z) >= 2)
                return true;

    const auto &ts = init.closed().t34s13();
    auto isYao = [](T34 t) { return t.isYao(); };
    return std::count_if(ts.begin(), ts.end(), isYao) <= 3;
}

void Sumire::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who == mSelf)
        handleDrawSelf(table, mount, rinshan);
    else if (mTarget.somebody() && who == mTarget)
        handleDrawTarget(table, mount, rinshan);
}

void Sumire::onIrsChecked(const Table &table, Mount &mount)
{
    if (mIrsCtrl.itemAt(MAIN).on()) {
        if (mIrsCtrl.itemAt(RIGHT).on())
            mTarget = mSelf.right();
        else if (mIrsCtrl.itemAt(CROSS).on())
            mTarget = mSelf.cross();
        else if (mIrsCtrl.itemAt(LEFT).on())
            mTarget = mSelf.left();
    }

    if (mTarget.somebody()) {
        bool allowNum19 = mIrsCtrl.itemAt(ALLOW_NUM19).on();
        bool allowZ = mIrsCtrl.itemAt(ALLOW_Z).on();
        bool planOk = chooseFinalWait(table, mount, allowNum19, allowZ)
                   && planAimming(table, mount);
        if (!planOk)
            mTarget = Who();

        table.popUp(mSelf);
        mIrsCtrl.setOnAt(IrsIndex::MAIN, false);
    }
}

std::string Sumire::popUpStr() const
{
    std::ostringstream oss;

    if (mTarget.somebody()) {
        if (!mFeedSelf.empty())
            oss << "DRAW" << mFeedSelf << std::endl;

        oss << "DISCARD" << mFinalWait;
    } else {
        oss << "AIM_FAILURE";
    }

    return oss.str();
}

void Sumire::onTableEvent(const Table &table, const TableEvent &event)
{
    if (event.type() != TableEvent::Type::DRAWN)
        return;

    auto args = event.as<TableEvent::Drawn>();
    if (args.who == mSelf && !mFeedSelf.empty()) {
        // consume one self feed if success
        T34 drawn = table.getHand(mSelf).drawn();
        if (auto it = std::find(mFeedSelf.begin(), mFeedSelf.end(), drawn); it != mFeedSelf.end()) {
            *it = mFeedSelf.back();
            mFeedSelf.popBack();
        }
    }
}

Girl::IrsCtrlGetter Sumire::attachIrsOnDrawn(const Table &table)
{
    if (canStartAiming(table)) {
        mIrsCtrl.setClickHost(table.getChoices(mSelf));
        return &Sumire::mIrsCtrl;
    }

    return nullptr;
}

void Sumire::handleDrawSelf(const Table &table, Mount &mount, bool rinshan)
{
    const Hand &hand = table.getHand(mSelf);
    if (mTarget.nobody()) {
        if (!hand.ready())
            shapeYaku(table, mount, rinshan);

        return;
    }

    if (hand.ready() && hand.hasEffA(mFinalWait))
        return;

    if (!mFeedSelf.empty()) {
        for (T34 t : tiles34::ALL34) {
            mount.lightA(t, util::has(mFeedSelf, t) ? 100 : -40, rinshan);
            mount.lightB(t, util::has(mFeedSelf, t) ? 100 : 0, rinshan);
        }
    }
}

void Sumire::handleDrawTarget(const Table &table, Mount &mount, bool rinshan)
{
    if (canShootTarget(table)) {
        // feed
        if (mShootTrial == 0) {
            for (T34 t : tiles34::ALL34) {
                mount.lightA(t, -500, rinshan);
                mount.lightB(t, t == mFeedTarget ? 100 : -100, rinshan);
            }

            mShootTrial++;
        } else if (mShootTrial == 1) {
            // TODO re-aim logic
            mShootTrial++;
        }
    } else if (!mFeedSelf.empty()) { // block target, give shit
        for (T34 t : tiles34::ALL34)
            if (!mTargetShits[size_t(t.id34())])
                mount.lightA(t, -40, rinshan);

        for (Suit s : { Suit::M, Suit::P, Suit::S })
            mount.lightA(T37(s, 0), -40, rinshan); // aka5 is usually not shit
    }
}

bool Sumire::canStartAiming(const Table &table)
{
    if (mTarget.somebody())
        return false;

    for (Who who : whos::ALL4) {
        const Hand &hand = table.getHand(who);

        if (who == mSelf) {
            bool dama = hand.isMenzen() && hand.ready()
                && !table.riichiEstablished(mSelf);
            if (!dama)
                return false;
        } else if (hand.ready()) {
            return false; // must be the first readyer
        }
    }

    return true;
}

bool Sumire::canShootTarget(const Table &table)
{
    if (mTarget.nobody())
        return false;

    const Hand &myHand = table.getHand(mSelf);
    bool dama = myHand.isMenzen() && myHand.ready() && !table.riichiEstablished(mSelf);

    return dama && myHand.hasEffA(mFinalWait);
}

bool Sumire::chooseFinalWait(const Table &table, Mount &mount, bool allowNum19, bool allowZ)
{
    int maxHappy = 0;
    mFinalWait = table.getHand(mSelf).effA().front();
    mTargetShits.reset();

    const Hand &taragetHand = table.getHand(mTarget);
    for (const T37 &feed : tiles37::ORDER37) {
        if (mount.remainA(feed) == 0)
            continue;

        Hand dream(taragetHand); // copy
        dream.draw(feed);
        Action act = Ai::thinkStdDrawnAttack(TableViewHand(table, dream, mTarget));
        if (act.isDiscard() || act.isRiichi()) {
            act = act.toDiscard();
            T34 discard = act.act() == ActCode::SWAP_OUT ? act.t37() : dream.drawn();
            if (act.act() == ActCode::SPIN_OUT)
                mTargetShits[size_t(feed.id34())] = true;

            if (util::has(table.getRiver(mSelf), discard))
                continue;

            if ((!allowNum19 && discard.isNum1928()) || (!allowZ && discard.isZ()))
                continue;

            if (int happy = evalTargetHappy(table, mount, dream, act); happy > maxHappy) {
                mFinalWait = discard;
                mFeedTarget = feed;
                maxHappy = happy;
            }
        }
    }

    if (maxHappy > 0) {
        mount.loadB(mFeedTarget, 1);
        return true;
    }

    return false;
}

///
/// \brief Compute target's happiness when given drawn-hand and action
/// \param dream A hand in "14 - 3k" status
/// \param act   A discard action
/// \return Happiness value, higher is happier
///
int Sumire::evalTargetHappy(const Table &table, const Mount &mount, const Hand &dream, const Action &act)
{
    int step = dream.peekDiscard(act, &Hand::step);
    auto effA = dream.peekDiscard(act, &Hand::effA);
    int effACt = table.visibleRemain(mTarget).ct(effA);
    int doraCt = mount.getDrids() % dream + dream.ctAka5();
    return 100 * (13 - step) + 3 * effACt + doraCt;
}

bool Sumire::planAimming(const Table &table, Mount &mount)
{
    const Hand &hand = table.getHand(mSelf);

    mFeedSelf.clear();
    if (hand.hasEffA(mFinalWait))
        return true; // too lucky, no need to aim

    if (hand.step7() == 0) {
        updateFeedSelf({ mFinalWait }); // simply replace waiter
    } else if (hand.step13() == 0) {
        // uncurable, go die, good bye
    } else if (Parsed4s parses = hand.parse4(); parses.step4() == 0) {
        for (const auto &parse : parses)
            updateFeedSelfParse(ParsedView4Ready(parse), mount);
    }

    for (T34 feed : mFeedSelf)
        mount.loadB(T37(feed.id34()), 1);

    return !mFeedSelf.empty();
}

void Sumire::updateFeedSelf(const util::Stactor<T34, 2> &plan)
{
    if (mFeedSelf.empty() || plan.size() < mFeedSelf.size())
        mFeedSelf = plan;
}

///
/// \brief Try to update the aiming plan with a given 4-meld ready parse
/// \pre 'view' should not already waiting 'mFinalWait'
///
void Sumire::updateFeedSelfParse(ParsedView4Ready view, Mount &mount)
{
    util::Stactor<C34, 2> comelds = view.get2s();
    if (comelds.empty()) { // isoride, simply replace
        updateFeedSelf({ mFinalWait });
    } else if (mFinalWait.isYao()) { // non-isoride, and waiting yao
        util::Stactor<T34, 2> frees = view.get1s();
        assert(comelds.size() == 2 && frees.size() == 1);
        updateFeedSelfYao(comelds, frees[0], mount);
    } else { // non-isoride and waiting non-yao, transform to clamp
        updateFeedSelfClamp(view, mount);
    }
}

void Sumire::updateFeedSelfYao(const util::Stactor<C34, 2> &comelds, T34 free, Mount &mount)
{
    size_t pairIndex = comelds[0].type() == C34::Type::PAIR ? 0 : 1;
    T34 pair = comelds[pairIndex].head();
    C34 another = comelds[1 - pairIndex]; // seq, or another bibump pair
    if (pair != mFinalWait) {
        if (mount.remainA(pair) > 0) {
            util::Stactor<T34, 2> feeds { pair };
            // transform to isoride form
            if (!another.has(mFinalWait) && mFinalWait != free)
                feeds.pushBack(mFinalWait);

            updateFeedSelf(feeds);
        }
    } else { // pair is final wait
        // transfer another seq to bibump pair
        if (!another.has(mFinalWait)) { // otherwise, 1113m form, give up
            for (T34 bibumpCandidate : another.t34s()) {
                if (mount.remainA(bibumpCandidate) > 0) {
                    updateFeedSelf({ bibumpCandidate });
                    break;
                }
            }
        }
    }
}

void Sumire::updateFeedSelfClamp(const ParsedView4Ready &view, Mount &mount)
{
    // *INDENT-OFF*
    auto missing = [&](T34 t) {
        auto inBreakableComeld = [t](const C34 &c) {
            return !c.is3() && c.has(t);
        };

        return util::none(view.getParsed().heads(), std::move(inBreakableComeld));
    };
    // *INDENT-ON*

    util::Stactor<T34, 2> clampers { mFinalWait.prev(), mFinalWait.next() };
    if (!util::all(clampers, [&mount](T34 t) { return mount.remainA(t) > 0; }))
        return;

    util::Stactor<T34, 2> newPlan;
    std::copy_if(clampers.begin(), clampers.end(), std::back_inserter(newPlan), std::move(missing));

    updateFeedSelf(newPlan);
}

void Sumire::shapeYaku(const Table &table, Mount &mount, bool rinshan)
{
    const Hand &hand = table.getHand(mSelf);

    if (!hand.isMenzen())
        return;

    if (hand.step() <= 2)
        accelerate(mount, hand, table.getRiver(mSelf), 30);

    shapeYakuhai(table, mount, rinshan);
}

bool Sumire::shapeYakuhai(const Table &table, Mount &mount, bool rinshan)
{
    const Hand &hand = table.getHand(mSelf);

    int sw = table.getSelfWind(mSelf);
    int rw = table.getRoundWind();
    // *INDENT-OFF*
    auto isPluralYakuhai = [&](T34 z) {
        return z.isYakuhai(sw, rw) && hand.closed().ct(z) >= 2;
    };
    // *INDENT-ON*

    const auto &z7 = tiles34::Z7;
    if (util::none(z7, isPluralYakuhai))
        return false;

    for (T34 z : z7)
        if (hand.closed().ct(z) == 2)
            mount.lightA(z, 500, rinshan);

    return true;
}



} // namespace saki
