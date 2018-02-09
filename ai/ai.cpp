#include "ai.h"

#include "ai_achiga_kuro.h"
#include "ai_eisui_hatsumi.h"
#include "ai_eisui_kasumi.h"
#include "ai_kiyosumi_nodoka.h"
#include "ai_miyamori_toyone.h"
#include "ai_shiraitodai_awai.h"
#include "ai_shiraitodai_seiko.h"
#include "ai_shiraitodai_takami.h"
#include "ai_usuzan_sawaya.h"

#include "../util/debug_cheat.h"
#include "../util/misc.h"



namespace saki
{



///
/// \brief Pick all max values according to 'measure'
///
util::Stactor<Action, 44> allMaxs(const util::Range<Action> &range,
                                  std::function<int(const Action &)> measure, int floor)
{
    int max = floor;
    util::Stactor<Action, 44> res;

    for (const auto &a : range) {
        int comax = measure(a);

        if (comax > max) {
            max = comax;
            res.clear();
        }

        if (comax == max)
            res.pushBack(a);
    }

    return res;
}

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



std::unique_ptr<Ai> Ai::create(Girl::Id id)
{
    switch (id) {
    case Girl::Id::SHIBUYA_TAKAMI:      return util::unique<AiTakami>();
    case Girl::Id::MATANO_SEIKO:        return util::unique<AiSeiko>();
    case Girl::Id::OOHOSHI_AWAI:        return util::unique<AiAwai>();
    case Girl::Id::MATSUMI_KURO:        return util::unique<AiKuro>();
    case Girl::Id::USUZUMI_HATSUMI:     return util::unique<AiHatsumi>();
    case Girl::Id::IWATO_KASUMI:        return util::unique<AiKasumi>();
    case Girl::Id::ANETAI_TOYONE:       return util::unique<AiToyone>();
    case Girl::Id::HARAMURA_NODOKA:     return util::unique<AiNodoka>();
    case Girl::Id::SHISHIHARA_SAWAYA:   return util::unique<AiSawaya>();
    default: return util::unique<Ai>();
    }
}

Action Ai::thinkStdDrawnAttack(const TableView &view)
{
    Ai ai;
    Limits limits;

    return ai.thinkDrawnAttack(view, limits);
}

TableDecider::Decision Ai::decide(const TableView &view)
{
    TableDecider::Decision decision;

#ifdef LIBSAKI_CHEAT_AI
    decision = placeHolder(*view);
#else
    if (view.me().irsReady())
        decision.action = thinkIrs(view);

    if (decision.action.act() == ActCode::NOTHING) {
        Limits limits;
        decision.action = think(view, limits);
    }

#endif

    assert(decision.action.act() != ActCode::NOTHING);
    return decision;
}

Action Ai::thinkIrs(const TableView &view)
{
    (void) view;
    unreached("unoverriden Ai::thinkIrs");
}

Action Ai::think(const TableView &view, Limits &limits)
{
    antiHatsumi(view, limits);
    antiToyone(view, limits);

    return thinkChoices(view, limits);
}

Action Ai::placeHolder(const TableView &view)
{
    return view.myChoices().timeout();
}

void Ai::antiHatsumi(const TableView &view, Ai::Limits &limits)
{
    Who hatsumi = view.findGirl(Girl::Id::USUZUMI_HATSUMI);
    if (hatsumi.nobody() || hatsumi == view.self() || view.getSelfWind(hatsumi) != 4)
        return;

    const auto &barks = view.getBarks(hatsumi);
    if (barks.size() == 1 || barks.size() == 2) {
        using namespace tiles34;

        bool hasE = util::any(barks, [](const M37 &m) { return m[0] == 1_f; });
        bool hasN = util::any(barks, [](const M37 &m) { return m[0] == 4_f; });

        if (hasE || hasN)
            limits.addNoRiichi();

        if (hasE != hasN)
            limits.addNoOut(hasE ? 4_f : 1_f);
    }
}

void Ai::antiToyone(const TableView &view, Ai::Limits &limits)
{
    Who toyone = view.findGirl(Girl::Id::ANETAI_TOYONE);
    if (toyone.somebody() && toyone != view.self() && view.isMenzen(toyone))
        limits.addNoRiichi();
}

Action Ai::thinkChoices(const TableView &view, Ai::Limits &limits)
{
    const Choices &choices = view.myChoices();

    switch (choices.mode()) {
    case Choices::Mode::WATCH:
        unreached("Ai::think: unexpected watch mode");
    case Choices::Mode::IRS_CHECK:
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
        unreached("Ai::thinkChoices: illegal mode");
    }
}

Action Ai::thinkDrawn(const TableView &view, Limits &limits)
{
    if (view.myChoices().can(ActCode::TSUMO))
        return Action(ActCode::TSUMO);

    return thinkDrawnAggress(view, limits);
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
    return outs.empty() ? placeHolder(view) : thinkAttackStep(view, outs.range());
}

Action Ai::thinkDrawnDefend(const TableView &view, Limits &limits, const util::Stactor<Who, 3> &threats)
{
    using AC = ActCode;

    if (view.myChoices().can(AC::RYUUKYOKU))
        return Action(AC::RYUUKYOKU);

    auto outs = listOuts(view, limits);
    return outs.empty() ? placeHolder(view) : thinkDefendChance(view, outs.range(), threats);
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
    int sw = view.getSelfWind(view.self());
    int rw = view.getRoundWind();

    if (hand.hasEffA(pick) && (barked || pick.isYakuhai(sw, rw)))
        return thinkAttackStep(view, listCp(hand, mode, pick).range());

    return Action(AC::PASS);
}

Action Ai::thinkBarkDefend(const TableView &view, Limits &limits,
                           const util::Stactor<Who, 3> &threats)
{
    if (limits.noBark())
        return Action(ActCode::PASS);

    if (view.getRule().ippatsu && view.inIppatsuCycle()) {
        const Hand &hand = view.myHand();
        const Choices::ModeBark &mode = view.myChoices().bark();
        const T37 &pick = view.getFocusTile();
        return thinkDefendChance(view, listCp(hand, mode, pick).range(), threats);
    }

    return Action(ActCode::PASS);
}

Action Ai::thinkAttackStep(const TableView &view, const util::Range<Action> &outs)
{
    assert(!outs.empty());
    assert(util::all(outs, [](const Action &a) { return a.isDiscard() || a.isCp(); }));

    // *INDENT-OFF*
    auto stepHappy = [&](const Action &action) {
        int step = action.isDiscard() ? view.myHand().peekDiscard(action, &Hand::step)
                                      : view.myHand().peekCp(view.getFocusTile(), action, &Hand::step);
        return 2 + (13 - step);
    };
    // *INDENT-ON*

    auto minSteps = allMaxs(outs, stepHappy, 0);
    if (minSteps.size() == 1)
        return minSteps[0];

    return thinkAttackEff(view, minSteps.range());
}

Action Ai::thinkAttackEff(const TableView &view, const util::Range<Action> &outs)
{
    assert(!outs.empty());
    assert(util::all(outs, [](const Action &a) { return a.isDiscard() || a.isCp(); }));

    // *INDENT-OFF*
    auto happy = [&](const Action &action) {
        const T37 &out = view.myHand().outFor(action);
        auto effA = action.isDiscard() ? view.myHand().peekDiscard(action, &Hand::effA)
                                       : view.myHand().peekCp(view.getFocusTile(), action, &Hand::effA);
        int remainEffA = view.visibleRemain().ct(effA);
        int floatTrash = (5 - (view.getDrids() % out + out.isAka5()))
                + 2 * (view.getRiver(view.self()).size() < 6 ? out.isYao() : !out.isYao());

        return 2 + 10 * remainEffA + floatTrash;
    };
    // *INDENT-ON*

    auto maxHappys = allMaxs(outs, happy, 0);
    return maxHappys[0];
}

Action Ai::thinkDefendChance(const TableView &view, const util::Range<Action> &outs,
                             const util::Stactor<Who, 3> &threats)
{
    assert(!outs.empty());
    assert(util::all(outs, [](const Action &a) { return a.isDiscard() || a.isCp(); }));

    // *INDENT-OFF*
    auto happy = [&](const Action &action) {
        const T37 &out = view.myHand().outFor(action);
        int cc = 0;
        for (Who who : threats)
            cc = std::max(cc, chance(view, who, out));
        int safe = 20 - cc;
        return 100 * safe + 10 * (view.getDrids() % out + out.isAka5()) + (34 - out.id34());
    };
    // *INDENT-ON*

    auto maxHappys = allMaxs(outs, happy, 0);
    assert(!maxHappys.empty());
    return maxHappys[0];
}

bool Ai::afraid(const TableView &view, util::Stactor<Who, 3> &threats)
{
    Who self = view.self();
    bool scary = false;

    if (!(view.riichiEstablished(self) || view.getRiver(self).size() < 6)) {
        for (int w = 0; w < 4; w++) {
            Who who(w);
            if (who == self)
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

    Action act = thinkAttackEff(view, outs.range());
    int est = view.myHand().peekDiscard(act, &Hand::estimate, view.getRule(),
                                        view.getSelfWind(view.self()), view.getRoundWind(), view.getDrids());

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

    if (!view.riichiEstablished(view.self()))
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
    if (limits.noRiichi())
        return res;

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
