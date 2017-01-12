#include "ai.h"
#include "util.h"

#include "ai_shiraitodai.h"
#include "ai_achiga.h"
#include "ai_senriyama.h"
#include "ai_eisui.h"
#include "ai_usuzan.h"



namespace saki
{



Ai *Ai::create(Who who, Girl::Id id)
{
    switch (id) {
    case Girl::Id::SHIBUYA_TAKAMI:      return new AiTakami(who);
    case Girl::Id::OOHOSHI_AWAI:        return new AiAwai(who);
    case Girl::Id::MATSUMI_KURO:        return new AiKuro(who);
    case Girl::Id::ONJOUJI_TOKI:        return new AiToki(who);
    case Girl::Id::IWATO_KASUMI:        return new AiKasumi(who);
    case Girl::Id::SHISHIHARA_SAWAYA:   return new AiSawaya(who);
    default:                            return new Ai(who);
    }
}

void Ai::onActivated(Table &table)
{
    TableView view(table.getView(mSelf));

    Action decision;

    if (view.myTickets().forwardAny())
        decision = forward(view);

    if (decision.act() == ActCode::NOTHING)
        decision = maxHappy(view);

    assert(decision.act() != ActCode::NOTHING);
    table.action(mSelf, decision);
}

Ai::Ai(Who who)
    : TableOperator(who)
{
}

Action Ai::maxHappy(const TableView &view)
{
    mPicture = Picture();

    std::vector<Action> choices = view.myChoices();
    for (int iter = 0; iter < 10; iter++) {
        int maxHappy = 0;
        std::vector<Action> maxActs;

        for (const Action &act : choices) {
            int comax = happy(view, iter, act);
            if (comax > maxHappy) {
                maxHappy = comax;
                maxActs.clear();
            }

            if (comax == maxHappy)
                maxActs.push_back(act);
        }

        choices = maxActs;
        if (choices.size() == 1)
            break;
    }

    assert(!choices.empty());
    return choices[0];
}

Action Ai::forward(const TableView &view)
{
    (void) view;
    unreached("unoverriden Ai::forward");
}

int Ai::happy(const TableView &view, int iter, const Action &action)
{
    assert(iter >= 0);
    assume_opt_out(iter >= 0);

    switch (iter) {
    case 0:     return happy0(view, action);
    case 1:     return happy1(view, action);
    case 2:     return happy2(view, action);
    case 3:     return happy3(view, action);
    default:    return 1;
    }
}

Action Ai::placeHolder(const TableView &view)
{
    if (view.iCan(ActCode::SPIN_OUT))
        return Action(ActCode::SPIN_OUT);
    if (view.iCan(ActCode::PASS))
        return Action(ActCode::PASS);

    return Action(ActCode::NOTHING);
}

int Ai::happy0(const TableView &view, const Action &action)
{
    (void) view;

    switch (action.act()) {
    case ActCode::END_TABLE:
        return 3;
    case ActCode::NEXT_ROUND:
    case ActCode::TSUMO:
    case ActCode::RON:
    case ActCode::DICE:
        return 2;
    default:
        return 1;
    }
}

int Ai::happy1(const TableView &view, const Action &action)
{
    if (mPicture.lastUpdate < 1) {
        if (view.riichiEstablished(mSelf) || view.getRiver(mSelf).size() < 6) {
            mPicture.defense = false;
        } else {
            for (int w = 0; w < 4; w++) {
                Who who(w);
                if (who == mSelf)
                    continue;
                int riverCt = view.getRiver(who).size();
                int barkCt = view.getBarks(who).size();
                if (view.riichiEstablished(who)
                        || (riverCt > 5 && barkCt >= 2)
                        || (riverCt > 12 && barkCt >= 1))
                    mPicture.threats.push_back(who);
            }

            if (mPicture.threats.empty()) {
                mPicture.defense = false;
            } else {
                int step = view.myHand().step();
                int doraCt = view.getDrids() % view.myHand() + view.myHand().ctAka5();
                mPicture.defense = step > 1 || doraCt < 2;
            }
        }

        mPicture.lastUpdate = 1;
    }

    return mPicture.defense ? happy1D(view, action) : happy1A(view, action);
}

int Ai::happy1D(const TableView &view, const Action &action)
{
    switch (action.act()) {
    case ActCode::RYUUKYOKU:
        return 2;
    case ActCode::CHII_AS_MIDDLE:
        return view.getRuleInfo().ippatsu && view.inIppatsuCycle() ? 5 : 0;
    case ActCode::CHII_AS_LEFT:
        return view.getRuleInfo().ippatsu && view.inIppatsuCycle() ? 4 : 0;
    case ActCode::CHII_AS_RIGHT:
        return view.getRuleInfo().ippatsu && view.inIppatsuCycle() ? 3 : 0;
    case ActCode::PASS:
        return 2;
    default:
        return 1;
    case ActCode::ANKAN:
    case ActCode::RIICHI:
    case ActCode::KAKAN:
    case ActCode::PON:
    case ActCode::DAIMINKAN:
        return 0;
    }
}

int Ai::happy1A(const TableView &view, const Action &action)
{
    bool barked = !view.myHand().isMenzen();
    int sw = view.getSelfWind();
    int rw = view.getRoundWind();

    switch (action.act()) {
    case ActCode::RIICHI:
        return riichi(view) ? 3 : 0;
    case ActCode::ANKAN:
        return !view.myHand().hasEffA(action.tile()) ? 2 : 0;
    case ActCode::PON:
        return (barked || view.getFocusTile().isYakuhai(sw, rw))
                && view.myHand().hasEffA(view.getFocusTile()) ? 6 : 0;
    case ActCode::CHII_AS_MIDDLE:
        return (barked && view.myHand().hasEffA(view.getFocusTile())) ? 5 : 0;
    case ActCode::CHII_AS_LEFT:
        return (barked && view.myHand().hasEffA(view.getFocusTile())) ? 4 : 0;
    case ActCode::CHII_AS_RIGHT:
        return (barked && view.myHand().hasEffA(view.getFocusTile())) ? 3 : 0;
    case ActCode::PASS:
        return 2;
    default:
        return 1;
    case ActCode::DAIMINKAN:
    case ActCode::KAKAN:
    case ActCode::RYUUKYOKU:
        return 0;
    }
}

int Ai::happy2(const TableView &view, const Action &action)
{
    if (mPicture.lastUpdate < 2) {
        // nothing to update yet now
        mPicture.lastUpdate = 2;
    }

    return mPicture.defense ? happy2D(view, action) : happy2A(view, action);
}

int Ai::happy2D(const TableView &view, const Action &action)
{
    assert(action.act() == ActCode::SWAP_OUT || action.act() == ActCode::SPIN_OUT);

    const T37 &out = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                       : view.myHand().drawn();
    int cc = 0;
    for (Who who : mPicture.threats)
        cc = std::max(cc, chance(view, who, out));
    int safe = 20 - cc;
    return 100 * safe + 10 * (view.getDrids() % out + out.isAka5()) + (34 - out.id34());
}

int Ai::happy2A(const TableView &view, const Action &action)
{
    HandDream dream = view.myHand().withAction(action);

    switch (action.act()) {
    case ActCode::SWAP_OUT:
    case ActCode::SPIN_OUT:
        return 2 + (13 - dream.step());
    default:
        unreached("Ai::happy2");
    }
}

int Ai::happy3(const TableView &view, const Action &action)
{
    assert(action.act() == ActCode::SWAP_OUT || action.act() == ActCode::SPIN_OUT);

    HandDream dream = view.myHand().withAction(action);
    const T37 &out = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                       : view.myHand().drawn();

    int remainEffA = view.visibleRemain().ct(dream.effA());
    int floatTrash = (5 - (view.getDrids() % out + out.isAka5()))
            + 2 * (view.getRiver(mSelf).size() < 6 ? out.isYao() : !out.isYao());

    return 2 + 10 * remainEffA + floatTrash;
}

bool Ai::riichi(const TableView &view)
{
    const Hand &hand = view.myHand();
    bool spinnable;
    std::vector<T37> swappables;
    hand.declareRiichi(swappables, spinnable);
    TicketFolder temp;
    if (!swappables.empty())
        temp.enableSwapOut(swappables);
    if (spinnable)
        temp.enable(ActCode::SPIN_OUT);
    // FUCK dangerous code:
    //      1. assuming calling happy3 does not smashes states
    //         sol: use list-ordered picture-update
    //      2. should make it more reduced and reused and safe
    //         such as maxHappy(TableView(view, temp)),
    std::vector<Action> acts = temp.choices();
    std::vector<int> happys;
    happys.resize(acts.size());
    std::transform(acts.begin(), acts.end(), happys.begin(),
                   [&](const Action &a) { return happy3(view, a); });
    auto maxi = std::max_element(happys.begin(), happys.end());
    const Action &act = acts[maxi - happys.begin()];
    HandDream dream = hand.withAction(act);

    int est = dream.estimate(view.getRuleInfo(),
                             view.getSelfWind(), view.getRoundWind(),
                             view.getDrids());

    return est < 7000;
}

int Ai::chance(const TableView &view, Who tar, T34 t)
{
    // 'min' because it is ok if one of rule or logic gives a reason
    return std::min(ruleChance(view, tar, t), logicChance(view, t));
}

int Ai::ruleChance(const TableView &view, Who tar, T34 t)
{
    if (view.genbutsu(tar, t))
        return 0;

    return 19; // 19 is maximum chance
}

int Ai::logicChance(const TableView &view, T34 t)
{
    std::vector<T34> waiters;
    waiters.reserve(5);

    waiters.push_back(t); // bibump and isoride cases

    if (t.isNum()) { // all possible neighbors
        if (t.val() > 1)
            waiters.push_back(t.prev());
        if (t.val() > 2)
            waiters.push_back(t.pprev());
        if (t.val() < 8)
            waiters.push_back(t.nnext());
        if (t.val() < 9)
            waiters.push_back(t.next());
    }

    return view.visibleRemain().ct(waiters);
}



} // namespace saki


