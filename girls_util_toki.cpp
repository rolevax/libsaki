#include "girls_util_toki.h"
#include "table.h"
#include "ai.h"
#include "string_enum.h"

#include <sstream>



namespace saki
{



TokiEvents::TokiEvents(const TokiEvents &copy)
{
    for (const std::unique_ptr<TokiEvent> &p : copy.events)
        events.emplace_back(p->clone());
}

TokiEvents &TokiEvents::operator=(TokiEvents assign)
{
    events.swap(assign.events);
    return *this;
}

void TokiEvents::emplace_back(TokiEvent *event)
{
    events.emplace_back(event);
}

std::string TokiEvents::str(Who toki) const
{
    bool inDiscardStream = false;

    std::ostringstream oss;

    for (const auto &p : events) {
        const TokiEvent &e = *p;
        if (e.isDiscard()) {
            if (!inDiscardStream) {
                oss << "\nDISCARD";
                inDiscardStream = true;
            }
        } else {
            inDiscardStream = false;
        }

        e.print(oss, toki);
    }

    return oss.str().substr(1); // eliminate first '\n'
}

bool TokiEvent::isDiscard() const
{
    return false;
}

TokiEventDrawn::TokiEventDrawn(const T37 &t)
    : mTile(t)
{
}

TokiEventDrawn *TokiEventDrawn::clone() const
{
    return new TokiEventDrawn(*this);
}

void TokiEventDrawn::print(std::ostream &os, Who toki) const
{
    (void) toki;
    os << "\nDRAW " << mTile.str();
}

TokiEventFlipped::TokiEventFlipped(const T37 &t)
    : mTile(t)
{
}

TokiEventFlipped *TokiEventFlipped::clone() const
{
    return new TokiEventFlipped(*this);
}

void TokiEventFlipped::print(std::ostream &os, Who toki) const
{
    (void) toki;
    os << "\nKANDORAINDIC " << mTile;
}

TokiEventDiscarded::TokiEventDiscarded(const T37 &t, bool spin, bool riichi)
    : mTile(t)
    , mSpin(spin)
    , mRiichi(riichi)
{
}

TokiEventDiscarded *TokiEventDiscarded::clone() const
{
    return new TokiEventDiscarded(*this);
}

bool TokiEventDiscarded::isDiscard() const
{
    return true;
}

void TokiEventDiscarded::print(std::ostream &os, Who toki) const
{
    (void) toki;
    os << (mSpin ? " *" : " ") << mTile.str() << (mRiichi ? "RII" : "");
}

TokiMountTracker::TokiMountTracker(Mount &mount, Who self)
    : mReal(mount)
    , mSelf(self)
{
}

TokiEventBarked::TokiEventBarked(Who who, const M37 &m)
    : mWho(who)
    , mBark(m)
{
}

TokiEventBarked *TokiEventBarked::clone() const
{
    return new TokiEventBarked(*this);
}

void TokiEventBarked::print(std::ostream &os, Who toki) const
{
    os << '\n' << mWho.turnFrom(toki) << "J " << saki::stringOf(mBark.type()) << ' ';
    if (mBark.isKan())
        os << T34(mBark[0]);
    else
        os << mBark;
}

TokiEventResult::TokiEventResult(RoundResult result,
                                 const std::vector<Who> &openers,
                                 const util::Stactor<util::Stactor<T37, 13>, 4> &closeds)
    : mResult(result)
    , mOpeners(openers)
    , mCloseds(closeds)
{
}

TokiEventResult::TokiEventResult(RoundResult result,
                                 const std::vector<Who> &openers,
                                 const util::Stactor<util::Stactor<T37, 13>, 4> &closeds,
                                 const T37 &pick,
                                 const util::Stactor<T37, 5> &urids)
    : mResult(result)
    , mOpeners(openers)
    , mCloseds(closeds)
    , mPick(pick)
    , mUrids(urids)
{
}

TokiEventResult *TokiEventResult::clone() const
{
    return new TokiEventResult(*this);
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
    if (table.getGirl(who).getId() == Girl::Id::ONJOUJI_TOKI)
        mEvents.emplace_back(new TokiEventDrawn(t));
}

void TokiMountTracker::onFlipped(const Table &table)
{
    // fix the mount
    const T37 &newIndic = table.getMount().getDrids().back();
    mReal.pin(Mount::DORA, mDoraPos++, newIndic);

    mEvents.emplace_back(new TokiEventFlipped(newIndic));
}

void TokiMountTracker::onDiscarded(const Table &table, bool spin)
{
    bool riichi = mToRiichi;
    if (riichi)
        mToRiichi = false;
    mEvents.emplace_back(new TokiEventDiscarded(table.getFocusTile(), spin, riichi));
}

void TokiMountTracker::onRiichiCalled(Who who)
{
    (void) who;
    mToRiichi = true;
}

void TokiMountTracker::onBarked(const Table &table, Who who, const M37 &bark, bool spin)
{
    (void) table;
    (void) spin;
    mEvents.emplace_back(new TokiEventBarked(who, bark));
}

void TokiMountTracker::onRoundEnded(const Table &table, RoundResult result,
                                    const std::vector<Who> &openers, Who gunner,
                                    const std::vector<Form> &fs)
{
    (void) gunner;
    (void) fs;

    util::Stactor<util::Stactor<T37, 13>, 4> closeds;
    for (Who w : openers)
        closeds.pushBack(table.getHand(w).closed().t37s13(true));

    if (result == RoundResult::RON || result == RoundResult::TSUMO) {
        bool ron = result == RoundResult::RON;
        const T37 &pick = ron ? table.getFocusTile()
                              : table.getHand(openers[0]).drawn();

        const auto &urids = table.getMount().getUrids();
        for (size_t i = 0; i < urids.size(); i++)
            mReal.pin(Mount::URADORA, i, urids[i]);

        mEvents.emplace_back(new TokiEventResult(result, openers, closeds, pick, urids));
    } else { // ryuukyoku
        mEvents.emplace_back(new TokiEventResult(result, openers, closeds));
    }
}

const TokiEvents &TokiMountTracker::getEvents() const
{
    return mEvents;
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
{
}

void TokiAutoOp::onActivated(Table &table)
{
    std::unique_ptr<TableView> view = table.getView(mSelf);
    Action decision = think(*view);
    if (decision.act() != ActCode::NOTHING)
        table.action(mSelf, decision);
}

Action TokiAutoOp::think(const TableView &view)
{
    if (mFirst) { // the test condition action
        mFirst = false;
        return mFirstAction;
    }

    if (view.myChoices().can(ActCode::PASS))
        return Action(ActCode::PASS);

    if (view.myChoices().can(ActCode::TSUMO))
        return Action(ActCode::TSUMO);

    return Action(); // halt the future table
}

void TokiEventResult::print(std::ostream &os, Who toki) const
{
    if (mResult == RoundResult::TSUMO || mResult == RoundResult::RON) {
        for (size_t w = 0; w < mOpeners.size(); w++) {
            os << '\n' << mOpeners[w].turnFrom(toki) << "J " << saki::stringOf(mResult)
               << '\n' << mCloseds[w];
        }
        if (!mUrids.empty())
            os << "\nURADORAINDIC " << mUrids;
    } else if (mResult == RoundResult::HP) {
        for (size_t w = 0; w < mOpeners.size(); w++) {
            os << '\n' << mOpeners[w].turnFrom(toki) << "J TENPAI "
               << '\n' << mCloseds[w];
        }
    } else {
        os << '\n' << saki::stringOf(mResult);
    }
}



} // namespace saki


