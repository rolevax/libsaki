#include "ai.h"
#include "util.h"

#include "ai_shiraitodai.h"
#include "ai_achiga.h"
#include "ai_miyamori.h"
#include "ai_senriyama.h"
#include "ai_eisui.h"
#include "ai_usuzan.h"
#include "debug_cheat.h"



namespace saki
{



bool Ai::Limits::noBark() const
{
    return mNoBark;
}

bool Ai::Limits::noRiichi() const
{
    return mNoRiichi;
}

bool Ai::Limits::noAnkan() const
{
    return mNoAnkan;
}

bool Ai::Limits::noOut(const T37 &t) const
{
    if (mNoOutAka5 && t.isAka5())
        return true;
    return mNoOut34s[t.id34()];
}

void Ai::Limits::addNoBark()
{
    mNoBark = true;
}

void Ai::Limits::addNoRiichi()
{
    mNoRiichi = true;
}

void Ai::Limits::addNoAnkan()
{
    mNoAnkan = true;
}

void Ai::Limits::addNoOutAka5()
{
    mNoOutAka5 = true;
}

void Ai::Limits::addNoOut(T34 t)
{
    mNoOut34s[t.id34()] = true;
}



Ai *Ai::create(Who who, Girl::Id id)
{
    switch (id) {
    case Girl::Id::SHIBUYA_TAKAMI:      return new AiTakami(who);
    case Girl::Id::MATANO_SEIKO:        return new AiSeiko(who);
    case Girl::Id::OOHOSHI_AWAI:        return new AiAwai(who);
    case Girl::Id::MATSUMI_KURO:        return new AiKuro(who);
    case Girl::Id::ONJOUJI_TOKI:        return new AiToki(who);
    case Girl::Id::USUZUMI_HATSUMI:     return new AiHatsumi(who);
    case Girl::Id::IWATO_KASUMI:        return new AiKasumi(who);
    case Girl::Id::ANETAI_TOYONE:       return new AiToyone(who);
    case Girl::Id::SHISHIHARA_SAWAYA:   return new AiSawaya(who);
    default:                            return new Ai(who);
    }
}

Action Ai::thinkStdDrawnAttack(const TableView &view)
{
    Ai ai(view.self());
    Limits limits;

    return ai.thinkDrawnAttack(view, limits);
}

void Ai::onActivated(Table &table)
{
    std::unique_ptr<TableView> view(table.getView(mSelf));

    Action decision;

#ifdef LIBSAKI_CHEAT_AI
    decision = placeHolder(*view);
#else
    if (view->myChoices().forwardAny())
        decision = forward(*view);

    if (decision.act() == ActCode::NOTHING) {
        Limits limits;
        decision = think(*view, limits);
    }
#endif

    assert(decision.act() != ActCode::NOTHING);
    table.action(mSelf, decision);
}

Ai::Ai(Who who)
    : TableOperator(who)
{
}

Action Ai::forward(const TableView &view)
{
    (void) view;
    unreached("unoverriden Ai::forward");
}

Action Ai::think(const TableView &view, Limits &limits)
{
    antiHatsumi(view, limits);
    antiToyone(view, limits);

    const Choices &choices = view.myChoices();

    switch (choices.mode()) {
    case Choices::Mode::WATCH:
        unreached("Ai::think: unexpected watch mode");
    case Choices::Mode::CUT:
        unreached("Ai::think: unhandled cut mode");
    case Choices::Mode::DICE:
        return Action(ActCode::DICE);
    case Choices::Mode::DRAWN:
        return thinkDrawn(view, limits);
    case Choices::Mode::BARK:
        return thinkBark(view, limits);
    case Choices::Mode::END:
        return Action(choices.can(ActCode::END_TABLE) ? ActCode::END_TABLE : ActCode::NEXT_ROUND);
    default:
        unreached("Ai::think: illegal mode");
    }
}

Action Ai::placeHolder(const TableView &view)
{
    return view.myChoices().sweep();
}

void Ai::antiHatsumi(const TableView &view, Ai::Limits &limits)
{
    Who hatsumi = view.findGirl(Girl::Id::USUZUMI_HATSUMI);
    if (hatsumi.nobody() || hatsumi == mSelf || view.getSelfWind(hatsumi) != 4)
        return;

    const auto &barks = view.getBarks(hatsumi);
    using namespace tiles34;
    if (barks.size() == 1 && (barks[0][0] == 1_f || barks[0][0] == 4_f)) {
        T34 another(Suit::F, 5 - barks[0][0].val());
        limits.addNoOut(another);
        limits.addNoRiichi();
    }
}

void Ai::antiToyone(const TableView &view, Ai::Limits &limits)
{
    Who toyone = view.findGirl(Girl::Id::ANETAI_TOYONE);
    if (toyone.somebody() && toyone != mSelf && view.isMenzen(toyone))
        limits.addNoRiichi();
}

Action Ai::thinkDrawn(const TableView &view, Limits &limit)
{
    if (view.myChoices().can(ActCode::TSUMO))
        return Action(ActCode::TSUMO);

    return thinkDrawnAggress(view, limit);
}

Action Ai::thinkDrawnAggress(const TableView &view, Limits &limits)
{
    util::Stactor<Who, 3> threats;
    return afraid(view, threats) ? thinkDrawnDefend(view, limits, threats) : thinkDrawnAttack(view, limits);
}

Action Ai::thinkDrawnAttack(const TableView &view, Limits &limits)
{
    using AC = ActCode;

    std::vector<Action> nexts;

    Action riichi;
    if (testRiichi(view, limits, riichi))
        return riichi;

    if (!limits.noAnkan())
        for (T34 t : view.myChoices().drawn().ankans)
            if (!view.myHand().hasEffA(t))
                return Action(AC::ANKAN, t);

    auto outs = listOuts(view, limits);
    return outs.empty() ? placeHolder(view) : thinkAttackStep(view, outs);
}

Action Ai::thinkDrawnDefend(const TableView &view, Limits &limits, const util::Stactor<Who, 3> &threats)
{
    using AC = ActCode;

    if (view.myChoices().can(AC::RYUUKYOKU))
        return Action(AC::RYUUKYOKU);

    auto outs = listOuts(view, limits);
    return outs.empty() ? placeHolder(view) : thinkDefendChance(view, outs, threats);
}

Action Ai::thinkBark(const TableView &view, Limits &limits)
{
    if (view.myChoices().can(ActCode::RON))
        return Action(ActCode::RON);

    util::Stactor<Who, 3> threats;
    return afraid(view, threats) ? thinkBarkDefend(view, limits, threats) : thinkBarkAttack(view, limits);
}

Action Ai::thinkBarkAttack(const TableView &view, Limits &limits)
{
    using AC = ActCode;

    if (limits.noBark())
        return Action(AC::PASS);

    const Hand &hand = view.myHand();
    const T37 &pick = view.getFocusTile();
    const Choices::ModeBark &mode = view.myChoices().bark();

    bool barked = !hand.isMenzen();
    int sw = view.getSelfWind(mSelf);
    int rw = view.getRoundWind();

    if (hand.hasEffA(pick) && (barked || pick.isYakuhai(sw, rw)))
        return thinkAttackStep(view, listCp(hand, mode, pick));

    return Action(AC::PASS);
}

Action Ai::thinkBarkDefend(const TableView &view, Limits &limits,
                            const util::Stactor<Who, 3> &threats)
{
    if (limits.noBark())
        return Action(ActCode::PASS);

    if (view.getRuleInfo().ippatsu && view.inIppatsuCycle()) {
        const Hand &hand = view.myHand();
        const Choices::ModeBark &mode = view.myChoices().bark();
        const T37 &pick = view.getFocusTile();
        return thinkDefendChance(view, listCp(hand, mode, pick), threats);
    }

    return Action(ActCode::PASS);
}

template<size_t MAX>
Action Ai::thinkAttackStep(const TableView &view, const util::Stactor<Action, MAX> &outs)
{
    assert(!outs.empty());
    assert(util::all(outs, [](const Action &a) { return a.isDiscard() || a.isCp(); }));

    auto stepHappy = [&](const Action &action) {
        int step = action.isDiscard() ? view.myHand().peekDiscard(action, &Hand::step)
                                      : view.myHand().peekCp(view.getFocusTile(), action, &Hand::step);
        return 2 + (13 - step);
    };

    auto minSteps = outs.maxs(stepHappy, 0);
    if (minSteps.size() == 1)
        return minSteps[0];

    return thinkAttackEff(view, minSteps);
}

template<size_t MAX>
Action Ai::thinkAttackEff(const TableView &view, const util::Stactor<Action, MAX> &outs)
{
    assert(!outs.empty());
    assert(util::all(outs, [](const Action &a) { return a.isDiscard() || a.isCp(); }));

    auto happy = [&](const Action &action) {
        const T37 &out = view.myHand().outFor(action);
        auto effA = action.isDiscard() ? view.myHand().peekDiscard(action, &Hand::effA)
                                       : view.myHand().peekCp(view.getFocusTile(), action, &Hand::effA);
        int remainEffA = view.visibleRemain().ct(effA);
        int floatTrash = (5 - (view.getDrids() % out + out.isAka5()))
                + 2 * (view.getRiver(mSelf).size() < 6 ? out.isYao() : !out.isYao());

        return 2 + 10 * remainEffA + floatTrash;
    };

    auto maxHappys = outs.maxs(happy, 0);
    return maxHappys[0];
}

template<size_t MAX>
Action Ai::thinkDefendChance(const TableView &view, const util::Stactor<Action, MAX> &outs,
                                    const util::Stactor<Who, 3> &threats)
{
    assert(!outs.empty());
    assert(util::all(outs, [](const Action &a) { return a.isDiscard() || a.isCp(); }));

    auto happy = [&](const Action &action) {
        const T37 &out = view.myHand().outFor(action);
        int cc = 0;
        for (Who who : threats)
            cc = std::max(cc, chance(view, who, out));
        int safe = 20 - cc;
        return 100 * safe + 10 * (view.getDrids() % out + out.isAka5()) + (34 - out.id34());
    };

    auto maxHappys = outs.maxs(happy, 0);
    assert(!maxHappys.empty());
    return maxHappys[0];
}

bool Ai::afraid(const TableView &view, util::Stactor<Who, 3> &threats)
{
    bool scary = false;

    if (!(view.riichiEstablished(mSelf) || view.getRiver(mSelf).size() < 6)) {
        for (int w = 0; w < 4; w++) {
            Who who(w);
            if (who == mSelf)
                continue;
            int riverCt = view.getRiver(who).size();
            int barkCt = view.getBarks(who).size();
            if (view.riichiEstablished(who)
                    || (riverCt > 5 && barkCt >= 2)
                    || (riverCt > 12 && barkCt >= 1))
                threats.pushBack(who);
        }

        if (!threats.empty()) {
            int step = view.myHand().step();
            int doraCt = view.getDrids() % view.myHand() + view.myHand().ctAka5();
            scary = step > 1 || doraCt < 1;
        }
    }

    return scary;
}

///
/// \param riichi will be set the best choice iff riichi is in choices
/// \return true iff riichi good for this situation
///
bool Ai::testRiichi(const TableView &view, Limits &limits, Action &riichi)
{
    auto outs = listRiichisAsOut(view.myHand(), view.myChoices().drawn(), limits);
    if (outs.empty())
        return false;

    Action act = thinkAttackEff(view, outs);
    int est = view.myHand().peekDiscard(act, &Hand::estimate, view.getRuleInfo(),
                                       view.getSelfWind(mSelf), view.getRoundWind(), view.getDrids());

    riichi = act.toRiichi();
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
    util::Stactor<T34, 5> waiters;

    waiters.pushBack(t); // bibump and isoride cases

    if (t.isNum()) { // all possible neighbors
        if (t.val() > 1)
            waiters.pushBack(t.prev());
        if (t.val() > 2)
            waiters.pushBack(t.pprev());
        if (t.val() < 8)
            waiters.pushBack(t.nnext());
        if (t.val() < 9)
            waiters.pushBack(t.next());
    }

    return view.visibleRemain().ct(waiters);
}

util::Stactor<Action, 14> Ai::listOuts(const TableView &view, const Limits &limits)
{
    util::Stactor<Action, 14> res;
    const Hand &hand = view.myHand();

    assert(hand.hasDrawn());
    if (!limits.noOut(hand.drawn()))
        res.pushBack(Action(ActCode::SPIN_OUT));

    if (!view.riichiEstablished(mSelf))
        for (const T37 &t : hand.closed().t37s13())
            if (!limits.noOut(t))
                res.pushBack(Action(ActCode::SWAP_OUT, t));

    return res;
}

util::Stactor<Action, 14> Ai::listRiichisAsOut(const Hand &hand, const Choices::ModeDrawn &mode,
                                               const Limits &limits)
{
    util::Stactor<Action, 14> res;

    assert(hand.hasDrawn());
    if (!limits.noOut(hand.drawn()) && mode.spinRiichi)
        res.pushBack(Action(ActCode::SPIN_OUT));

    for (const T37 &t : mode.swapRiichis)
        if (!limits.noOut(t))
            res.pushBack(Action(ActCode::SWAP_OUT, t));

    return res;
}

util::Stactor<Action, 44> Ai::listCp(const Hand &hand, const Choices::ModeBark &mode,
                                     const T37 &pick, bool noChii)
{
    util::Stactor<Action, 44> res;

    for (const T37 &out : tiles37::ORDER37) {
        if (hand.closed().ct(out) >= 1) {
            if (!noChii) {
                if (mode.chiiL) {
                    Action act(ActCode::CHII_AS_LEFT, 2, out);
                    if (hand.canCp(pick, act))
                        res.pushBack(act);
                }

                if (mode.chiiM) {
                    Action act(ActCode::CHII_AS_MIDDLE, 2, out);
                    if (hand.canCp(pick, act))
                        res.pushBack(act);
                }

                if (mode.chiiR) {
                    Action act(ActCode::CHII_AS_RIGHT, 2, out);
                    if (hand.canCp(pick, act))
                        res.pushBack(act);
                }
            }

            if (mode.pon) {
                Action act(ActCode::PON, 2, out);
                if (hand.canCp(pick, act))
                    res.pushBack(act);
            }
        }
    }

    return res;
}



} // namespace saki


