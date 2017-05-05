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

std::vector<Action> Ai::filter(const std::vector<Action> &orig,
                               std::function<bool(const Action &)> pass)
{
    std::vector<Action> res(orig.size());

    auto it = std::copy_if(orig.begin(), orig.end(), res.begin(), pass);
    res.resize(std::distance(res.begin(), it));

    return res;
}

void Ai::onActivated(Table &table)
{
    TableView view(table.getView(mSelf));

    Action decision;

#ifdef LIBSAKI_CHEAT_AI
    decision = placeHolder(view);
#else
    if (view.iForwardAny())
        decision = forward(view);

    if (decision.act() == ActCode::NOTHING) {
        std::vector<Action> choices = view.myChoices();
        decision = think(view, choices);
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

Action Ai::think(const TableView &view, const std::vector<Action> &choices)
{
    if (choices.empty()) {
        util::p("Ai.think: get empty choices");
        return placeHolder(view);
    }

    Action res = thinkTrivial(choices);
    if (res.act() != ActCode::NOTHING)
        return res;

    Who hatsumi = view.findGirl(Girl::Id::USUZUMI_HATSUMI);
    if (hatsumi.somebody() && hatsumi != mSelf && view.getSelfWind(hatsumi) == 4) {
        const std::vector<M37> &barks = view.getBarks(hatsumi);
        using namespace tiles34;
        if (barks.size() == 1 && (barks[0][0] == 1_f || barks[0][0] == 4_f)) {
            T34 another(Suit::F, 5 - barks[0][0].val());
            auto pass = [another, &view](const Action &act) {
                if (act.act() == ActCode::SWAP_OUT)
                    return act.tile() != another;
                else if (act.act() == ActCode::SPIN_OUT)
                    return view.myHand().drawn() != another;
                else if (act.act() == ActCode::RIICHI)
                    return false;
                else
                    return true;
            };

            std::vector<Action> next = filter(choices, pass);
            if (!next.empty())
                return thinkAggress(next, view);
        }
    }

    return thinkAggress(choices, view);
}

Action Ai::placeHolder(const TableView &view)
{
    return view.mySweep();
}

Action Ai::thinkTrivial(const std::vector<Action> &choices)
{
    using AC = ActCode;

    for (Action act : choices)
        if (act.act() == AC::END_TABLE)
            return act;

    for (Action act : choices) {
        AC a = act.act();
        if (a == AC::NEXT_ROUND || a == AC::TSUMO || a == AC::RON || a == AC::DICE)
            return act;
    }

    return Action();
}

Action Ai::thinkAggress(const std::vector<Action> &choices, const TableView &view)
{
    bool defense = false;
    std::vector<Who> threats;

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
                threats.push_back(who);
        }

        if (!threats.empty()) {
            int step = view.myHand().step();
            int doraCt = view.getDrids() % view.myHand() + view.myHand().ctAka5();
            defense = step > 1 || doraCt < 1;
        }
    }

    return defense ? thinkDefense(choices, view, threats)
                   : thinkAttack(choices, view);
}

Action Ai::thinkDefense(const std::vector<Action> &choices, const TableView &view,
                        const std::vector<Who> &threats)
{
    using AC = ActCode;

    bool pass = false;
    std::vector<Action> nexts;
    for (const Action &act : choices) {
        AC a = act.act();
        if (a == AC::RYUUKYOKU)
            return act;
        if (act.isChii() && view.getRuleInfo().ippatsu && view.inIppatsuCycle())
            return act;
        if (a == AC::PASS)
            pass = true;
        if (act.isDiscard())
            nexts.emplace_back(act);
    }

    if (pass)
        return Action(AC::PASS);

    return nexts.empty() ? placeHolder(view) : thinkDefenseDiscard(nexts, view, threats);
}

Action Ai::thinkDefenseDiscard(const std::vector<Action> &choices, const TableView &view,
                               const std::vector<Who> &threats)
{
    assert(!choices.empty());
    assert(util::all(choices, [](const Action &a) { return a.isDiscard(); }));

    auto happy = [&](const Action &action) {
        const T37 &out = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                           : view.myHand().drawn();
        int cc = 0;
        for (Who who : threats)
            cc = std::max(cc, chance(view, who, out));
        int safe = 20 - cc;
        return 100 * safe + 10 * (view.getDrids() % out + out.isAka5()) + (34 - out.id34());
    };

    std::vector<int> happys;
    happys.resize(choices.size());
    std::transform(choices.begin(), choices.end(), happys.begin(), happy);

    auto it = std::max_element(happys.begin(), happys.end());
    return choices[it - happys.begin()];
}

Action Ai::thinkAttack(const std::vector<Action> &choices, const TableView &view)
{
    using AC = ActCode;

    bool barked = !view.myHand().isMenzen();
    int sw = view.getSelfWind(mSelf);
    int rw = view.getRoundWind();

    // drunk ai, making no sense, nearyly random, don't read it.
    // just too lazy to write a good one.
    for (const Action &act : choices) {
        if (act.act() == AC::PON) {
            if ((barked || view.getFocusTile().isYakuhai(sw, rw))
                    && view.myHand().hasEffA(view.getFocusTile()))
                return act;
        }

        if (act.isChii()) {
            if (barked && view.myHand().hasEffA(view.getFocusTile()))
                return act;
        }
    }

    for (const Action &act : choices)
        if (act.act() == AC::PASS)
            return act;

    std::vector<Action> nexts;

    for (const Action &act : choices) {
        if (act.act() == AC::RIICHI && riichi(view))
            return act;

        if (act.act() == AC::ANKAN && !view.myHand().hasEffA(act.tile()))
            return act;

        if (act.isDiscard())
            nexts.emplace_back(act);
    }

    return nexts.empty() ? placeHolder(view) : thinkAttackDiscard(nexts, view);
}

Action Ai::thinkAttackDiscard(std::vector<Action> &choices, const TableView &view)
{
    // drunk ai, just a placeholder, don't read it
    assert(!choices.empty());
    assert(util::all(choices, [](const Action &a) { return a.isDiscard(); }));

    auto stepHappy = [&](const Action &action) {
        HandDream dream = view.myHand().withAction(action);
        return 2 + (13 - dream.step());
    };

    std::vector<Action> minSteps = util::maxs(choices, stepHappy, 0);
    if (minSteps.size() == 1)
        return minSteps[0];

    return thinkAttackDiscardWide(minSteps, view);
}

Action Ai::thinkAttackDiscardWide(std::vector<Action> &choices, const TableView &view)
{
    // drunk ai, just a placeholder, don't read it
    assert(!choices.empty());
    assert(util::all(choices, [](const Action &a) { return a.isDiscard(); }));

    auto happy = [&](const Action &action) {
        HandDream dream = view.myHand().withAction(action);
        const T37 &out = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                           : view.myHand().drawn();

        int remainEffA = view.visibleRemain().ct(dream.effA());
        int floatTrash = (5 - (view.getDrids() % out + out.isAka5()))
                + 2 * (view.getRiver(mSelf).size() < 6 ? out.isYao() : !out.isYao());

        return 2 + 10 * remainEffA + floatTrash;
    };

    std::vector<Action> maxHappys = util::maxs(choices, happy, 0);
    return maxHappys[0];
}

bool Ai::riichi(const TableView &view)
{
    Who toyone = view.findGirl(Girl::Id::ANETAI_TOYONE);
    if (toyone.somebody() && toyone != mSelf && view.isMenzen(toyone))
        return false;

    const Hand &hand = view.myHand();
    bool spinnable;
    std::vector<T37> swappables;
    hand.declareRiichi(swappables, spinnable);
    std::vector<Action> acts;
    for (const T37 &t : swappables)
        acts.emplace_back(ActCode::SWAP_OUT, t);
    if (spinnable)
        acts.emplace_back(ActCode::SPIN_OUT);

    Action act = thinkAttackDiscardWide(acts, view);
    HandDream dream = hand.withAction(act);
    int est = dream.estimate(view.getRuleInfo(),
                             view.getSelfWind(mSelf), view.getRoundWind(),
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


