#include "girls_util_toki.h"
#include "table.h"
#include "ai.h"



namespace saki
{



TokiMountTracker::TokiMountTracker(Mount &mount, Who self)
    : mReal(mount)
    , mSelf(self)
{

}

void TokiMountTracker::onDrawn(const Table &table, Who who)
{
    // fix the mount.
    // fixing random generator is not enough,
    // because the mount should appears the same even order is different
    const T37 &t = table.getHand(who).drawn();
    mReal.pin(table.duringKan() ? Mount::DEAD : Mount::WALL,
              table.duringKan() ? mDeadPos++ : mWallPos++, t);

    // see self's draw, output to expr
    if (table.getGirl(who).getId() == Girl::Id::ONJOUJI_TOKI) {
        mExpr.push_back(SkillToken::DRAW);
        mExpr.push_back(tokenOf(t));
    }
}

void TokiMountTracker::onFlipped(const Table &table)
{
    // fix the mount
    const T37 &newIndic = table.getMount().getDrids().back();
    mReal.pin(Mount::DORA, mDoraPos++, newIndic);

    mExpr.push_back(SkillToken::KANDORA_I);
    mExpr.push_back(tokenOf(newIndic));
}

void TokiMountTracker::onDiscarded(const Table &table, bool spin)
{
    mExpr.push_back(spin ? SkillToken::TMKR : SkillToken::TKR);
    mExpr.push_back(tokenOf(table.getFocusTile()));
}

void TokiMountTracker::onRiichiCalled(Who who)
{
    (void) who;
    mExpr.push_back(SkillToken::RIICHI);
}

void TokiMountTracker::onBarked(const Table &table, Who who, const M37 &bark)
{
    (void) table;

    mExpr.push_back(tokenOf(who));
    mExpr.push_back(tokenOf(bark.type()));
    if (bark.type() == M37::Type::CHII || bark.type() == M37::Type::PON) {
        // record all 3 tiles to note whether there is akadora
        for (int i = 0; i < 3; i++)
            mExpr.push_back(tokenOf(bark[i]));
    } else {
        // kan cases, one is enough
        mExpr.push_back(tokenOf(bark[0]));
    }
}

void TokiMountTracker::onRoundEnded(const Table &table, RoundResult result,
                                    const std::vector<Who> &openers, Who gunner,
                                    const std::vector<Form> &fs)
{
    (void) gunner;
    (void) fs;

    if (result == RoundResult::RON || result == RoundResult::TSUMO) {
        bool ron = result == RoundResult::RON;
        const T37 &pick = ron ? table.getFocusTile()
                              : table.getHand(openers[0]).drawn();

        for (Who w : openers) {
            mExpr.push_back(tokenOf(w));
            // is form[0] is ron, all is ron
            mExpr.push_back(tokenOf(result));
            mExpr.push_back(tokenOf(pick));
            for (const T37 &t : table.getHand(w).closed().t37s(true))
                mExpr.push_back(tokenOf(t));
        }

        // show and fix uradora indicators
        if (table.getRuleInfo().uradora) {
            const std::vector<T37> &uraIndics = table.getMount().getUrids();
            if (!uraIndics.empty()) {
                mExpr.push_back(SkillToken::URADORA_I);
                for (size_t i = 0; i < uraIndics.size(); i++) {
                    const T37 &ind = uraIndics[i];
                    mExpr.push_back(tokenOf(ind));
                    mReal.pin(Mount::URADORA, i, ind);
                }
            }
        }
    } else { // ryuukyoku
        mExpr.push_back(tokenOf(result));

        if (!openers.empty()) {
            for (Who w : openers) {
                if (table.getGirl(w).getId() != Girl::Id::ONJOUJI_TOKI) {
                    mExpr.push_back(tokenOf(w));
                    for (const T37 &t : table.getHand(w).closed().t37s(true))
                        mExpr.push_back(tokenOf(t));
                }
            }
        }
    }
}

const SkillExpr &TokiMountTracker::getExpr() const
{
    return mExpr;
}



TokiAutoOp::FourOps TokiAutoOp::create(const std::array<Girl::Id, 4> &ids,
                                                      const Action &firstAction)
{
    std::array<std::unique_ptr<TableOperator>, 4> res;

    for (int w = 0; w < 4; w++) {
        if (ids[w] == Girl::Id::ONJOUJI_TOKI)
            res[w].reset(new TokiAutoOp(Who(w), firstAction));
        else
            res[w].reset(Ai::create(Who(w), ids[w]));
    }

    return res;
}

TokiAutoOp::TokiAutoOp(Who self, const Action &firstAction)
    : TableOperator(self)
    , mFirstAction(firstAction)
    , mDiscarded(firstAction.act() == ActCode::SWAP_OUT
                 || firstAction.act() == ActCode::SPIN_OUT
                 || firstAction.act() == ActCode::PASS)
{

}

void TokiAutoOp::onActivated(Table &table)
{
    Action decision = think(table.getView(mSelf));
    if (decision.act() != ActCode::NOTHING)
        table.action(mSelf, decision);
}

Action TokiAutoOp::think(const TableView &view)
{
    if (mFirst) { // the test condition action
        mFirst = false;
        return mFirstAction;
    }

    if (view.iCan(ActCode::PASS))
        return Action(ActCode::PASS);

    if (view.iCan(ActCode::TSUMO))
        return Action(ActCode::TSUMO);

    if (!mDiscarded && view.iCan(ActCode::SWAP_OUT)) { // discard after bark
        mDiscarded = true;
        return Action(ActCode::SWAP_OUT, view.mySwappables().at(0));
    }

    return Action(); // halt the future table
}



} // namespace saki


