#include "girls_senriyama.h"
#include "table.h"
#include "ai.h"
#include "form.h"
#include "string_enum.h"
#include "util.h"

#include <algorithm>
#include <cassert>



namespace saki
{




bool Toki::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    return who != mSelf || iter > 10 || init.step4() <= 4;
}

void Toki::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 25);
}

void Toki::onActivate(const Table &table, Choices &choices)
{
    (void) table;

    if (choices.can(ActCode::DICE)
            || choices.can(ActCode::NEXT_ROUND)
            || choices.can(ActCode::END_TABLE)
            || choices.spinOnly())
        return; // not a branch point

    if (mCd > 0) {
        mCd--;
    } else {
        mCleanChoices = choices;
        choices.setExtra(true);
        mCrazyChoices = choices;
        mCrazyChoices.setForwarding(true);
    }
}

void Toki::onInbox(Who who, const Action &action)
{
    using AC = ActCode;

    // range of care, excluding DICE, and IRS
    static const std::vector<ActCode> CHECK {
        AC::PASS, AC::SWAP_OUT, AC::SPIN_OUT, AC::SWAP_RIICHI, AC::SPIN_RIICHI,
        AC::CHII_AS_LEFT, AC::CHII_AS_MIDDLE, AC::CHII_AS_RIGHT,
        AC::PON, AC::DAIMINKAN, AC::ANKAN, AC::KAKAN, AC::TSUMO, AC::RON, AC::RYUUKYOKU
    };

    if (who != mSelf || !mCheckNextAction)
        return;

    if (util::has(CHECK, action.act())) {
        mCheckNextAction = false;

        auto it = std::find(mRecords.begin(), mRecords.end(), action);
        mCd = 2 * (it - mRecords.begin());
        mRecords.clear();
    }
}

Choices Toki::forwardAction(const Table &table, Mount &mount, const Action &action)
{
    if (action.act() == ActCode::IRS_CLICK) {
        if (mInFuture) { // to exit future
            mInFuture = false;
            mCheckNextAction = true;
            return mCleanChoices;
        } else { // to enter future
            mInFuture = true;
            return mCrazyChoices;
        }
    }

    popUpBy(table, PopUpMode::OO);

    // prepare operators
    std::array<Girl::Id, 4> ids;
    for (int w = 0; w < 4; w++)
        ids[w] = table.getGirl(Who(w)).getId();
    std::array<std::unique_ptr<TableOperator>, 4> ais = TokiAutoOp::create(ids, action);
    std::array<TableOperator*, 4> operators;
    std::transform(ais.begin(), ais.end(), operators.begin(),
                   [](std::unique_ptr<TableOperator> &up) { return up.get(); });

    // prepare observer
    TokiMountTracker mountTracker(mount, mSelf);
    std::vector<TableObserver*> observers { &mountTracker };

    Table future(table, operators, observers, mSelf, mCleanChoices);
    future.start();

    // push iff not found
    if (!util::has(mRecords, action))
        mRecords.push_back(action);

    mEvents = mountTracker.getEvents();
    popUpBy(table, PopUpMode::FV);
    return mCrazyChoices;
}

std::string Toki::popUpStr() const
{
    switch (mPopUpMode) {
    case PopUpMode::OO:
        return std::string("----  O.O  ----");
    case PopUpMode::FV:
        return mEvents.str(mSelf);
    default:
        unreached("Toki::popUpStr");
    }
}

void Toki::popUpBy(const Table &table, Toki::PopUpMode mode)
{
    mPopUpMode = mode;
    table.popUp(mSelf);
}



void Sera::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan || table.inIppatsuCycle())
        return;

    const Hand &hand = table.getHand(mSelf);
    const PointInfo &info = table.getPointInfo(mSelf);
    const RuleInfo &rule = table.getRuleInfo();
    const auto &drids = mount.getDrids();
    if (hand.ready()) {
        for (T34 t : hand.effA()) {
            Form f(hand, T37(t.id34()), info, rule, drids);
            int ronHan = f.han();
            int tsumoHan = hand.isMenzen() ? ronHan + 1 : ronHan;
            bool pinfu = f.yakus().test(Yaku::PF);
            bool ok = tsumoHan >= (4 + pinfu);
            bool modest = tsumoHan <= 7;
            mount.lightA(t, ok ? (modest ? 400 : 100) : -200);
        }
    } else {
        accelerate(mount, hand, table.getRiver(mSelf), 30);
        if (hand.ctAka5() + drids % hand < 2) {
            for (const T37 &t : drids)
                mount.lightA(t.dora(), 80);
            mount.lightA(T37(Suit::M, 0), 30);
            mount.lightA(T37(Suit::P, 0), 30);
            mount.lightA(T37(Suit::S, 0), 30);
        }
    }
}



} // namespace saki


