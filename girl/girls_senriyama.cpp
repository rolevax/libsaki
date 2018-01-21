#include "girls_senriyama.h"
#include "../table/table.h"
#include "../ai/ai.h"
#include "../form/form.h"
#include "../util/string_enum.h"
#include "../util/misc.h"



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

Girl::IrsCtrlGetter Toki::attachIrsOnDrawn(const Table &table)
{
    const Choices &choices = table.getChoices(mSelf);

    if (mCd > 0) {
        mCd--;
        return nullptr;
    }

    mIrsCtrl.setClickHost(choices);
    return &Toki::mIrsCtrl;
}

std::array<Girl::Id, 4> Toki::makeIdArray(const Table &table)
{
    std::array<Girl::Id, 4> ids;

    for (int w = 0; w < 4; w++)
        ids[w] = table.getGirl(Who(w)).getId();

    return ids;
}

const Choices &Toki::PredictCtrl::choices() const
{
    return mChoices;
}

IrsResult Toki::PredictCtrl::handle(Toki &toki, const Table &table, Mount &mount, const Action &action)
{
    return toki.handleIrs(table, mount, action);
}

void Toki::PredictCtrl::setClickHost(Choices normal)
{
    mChoices = normal;
    mChoices.setExtra(true);
}

IrsResult Toki::handleIrs(const Table &table, Mount &mount, const Action &action)
{
    if (!mInFuture && !action.isIrs())
        return { false, nullptr };

    if (action.act() == ActCode::IRS_CLICK)
        return handleIrsClick();

    return handlePredict(table, mount, action);
}

IrsResult Toki::handleIrsClick()
{
    // to exit future
    if (mInFuture) {
        mInFuture = false;
        mCheckNextAction = true;
        return { true, nullptr };
    }

    // to enter future
    mInFuture = true;
    return { true, &Toki::mIrsCtrl };
}

IrsResult Toki::handlePredict(const Table &table, Mount &mount, const Action &action)
{
    popUpBy(table, PopUpMode::OO);

    // clone the table, and attach a mount tracker
    TokiMountTracker mountTracker(mount, mSelf);
    Table future(table, { &mountTracker });

    // run simulation
    TokiHumanSimulator ths(action, makeIdArray(table));
    TableTester tester(future, ths.makeDeciders());
    tester.run(true);

    if (!util::has(mRecords, action))
        mRecords.push_back(action);

    mEvents = mountTracker.getEvents();
    popUpBy(table, PopUpMode::FV);

    return { true, &Toki::mIrsCtrl };
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
    const FormCtx &ctx = table.getFormCtx(mSelf);
    const Rule &rule = table.getRule();
    const auto &drids = mount.getDrids();
    if (hand.ready()) {
        for (T34 t : hand.effA()) {
            Form f(hand, T37(t.id34()), ctx, rule, drids);
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
