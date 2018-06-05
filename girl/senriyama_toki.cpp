#include "senriyama_toki.h"
#include "../table/table.h"
#include "../table/table_tester.h"
#include "../ai/ai.h"
#include "../form/form.h"
#include "../util/string_enum.h"
#include "../util/misc.h"



namespace saki
{



class TokiEvent
{
public:
    static std::string stringOf(const std::vector<std::unique_ptr<TokiEvent>> &events);

    virtual TokiEvent *clone() const = 0;
    virtual ~TokiEvent() = default;
    virtual bool isDiscard() const;
    virtual void print(std::ostream &os, Who toki) const = 0;
};

struct TokiEvents
{
    TokiEvents() = default;
    TokiEvents(const TokiEvents &copy);
    TokiEvents &operator=(TokiEvents assign);
    void emplace_back(TokiEvent *event);
    std::string str(Who toki) const;
    std::vector<std::unique_ptr<TokiEvent>> events;
};



class TokiEventDrawn : public TokiEvent
{
public:
    explicit TokiEventDrawn(const T37 &t);
    explicit TokiEventDrawn(const TokiEventDrawn &copy) = default;
    TokiEventDrawn *clone() const override;
    void print(std::ostream &os, Who toki) const override;

private:
    T37 mTile;
};

class TokiEventFlipped : public TokiEvent
{
public:
    explicit TokiEventFlipped(const T37 &t);
    explicit TokiEventFlipped(const TokiEventFlipped &copy) = default;
    TokiEventFlipped *clone() const override;
    void print(std::ostream &os, Who toki) const override;

private:
    T37 mTile;
};

class TokiEventDiscarded : public TokiEvent
{
public:
    explicit TokiEventDiscarded(const T37 &t, bool spin, bool riichi = false);
    explicit TokiEventDiscarded(const TokiEventDiscarded &copy) = default;
    TokiEventDiscarded *clone() const override;
    bool isDiscard() const override;
    void print(std::ostream &os, Who toki) const override;

private:
    T37 mTile;
    bool mSpin;
    bool mRiichi;
};

class TokiEventBarked : public TokiEvent
{
public:
    explicit TokiEventBarked(Who who, const M37 &m);
    explicit TokiEventBarked(const TokiEventBarked &copy) = default;
    TokiEventBarked *clone() const override;
    void print(std::ostream &os, Who toki) const override;

private:
    Who mWho;
    M37 mBark;
};

class TokiEventResult : public TokiEvent
{
public:
    TokiEventResult(RoundResult result,
                    const util::Stactor<Who, 4> &openers,
                    const util::Stactor<util::Stactor<T37, 13>, 4> &closeds);
    TokiEventResult(RoundResult result,
                    const util::Stactor<Who, 4> &openers,
                    const util::Stactor<util::Stactor<T37, 13>, 4> &closeds,
                    const T37 &pick,
                    const util::Stactor<T37, 5> &urids);
    explicit TokiEventResult(const TokiEventResult &copy) = default;
    TokiEventResult *clone() const override;
    void print(std::ostream &os, Who toki) const override;

private:
    RoundResult mResult;
    util::Stactor<Who, 4> mOpeners;
    util::Stactor<util::Stactor<T37, 13>, 4> mCloseds;
    T37 mPick;
    util::Stactor<T37, 5> mUrids;
};

class TokiMountTracker : public TableObserverDispatched
{
public:
    explicit TokiMountTracker(Mount &mount, Who self);

    explicit TokiMountTracker(const TokiMountTracker &copy) = delete;
    TokiMountTracker &operator=(const TokiMountTracker &assign) = delete;

    void onTableEvent(const Table &table, const TE::Drawn &event) override;
    void onTableEvent(const Table &table, const TE::Flipped &event) override;
    void onTableEvent(const Table &table, const TE::Discarded &event) override;
    void onTableEvent(const Table &table, const TE::RiichiCalled &event) override;
    void onTableEvent(const Table &table, const TE::Barked &event) override;
    void onTableEvent(const Table &table, const TE::RoundEnded &event) override;

    const TokiEvents &getEvents() const;

private:
    Mount &mReal;
    Who mSelf;
    size_t mWallPos = 0; // wall position counter
    size_t mDeadPos = 0; // dead wall position counter
    size_t mDoraPos = 0;
    bool mToRiichi = false;
    TokiEvents mEvents;
};



class TokiHumanSimulator : public TableDecider
{
public:
    TokiHumanSimulator(const Action &firstAction,
                       const std::array<Girl::Id, 4> &ids);

    std::array<TableDecider *, 4> makeDeciders();

    Decision decide(const TableView &view) override;

private:
    Action think(const TableView &view);

private:
    bool mFirst = true;
    Action mFirstAction;
    std::array<std::unique_ptr<TableDecider>, 4> mAis;
};



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
    os << '\n' << mWho.turnFrom(toki) << "J " << util::stringOf(mBark.type()) << ' ';
    if (mBark.isKan())
        os << T34(mBark[0]);
    else
        os << mBark;
}

TokiEventResult::TokiEventResult(RoundResult result,
                                 const util::Stactor<Who, 4> &openers,
                                 const util::Stactor<util::Stactor<T37, 13>, 4> &closeds)
    : mResult(result)
    , mOpeners(openers)
    , mCloseds(closeds)
{
}

TokiEventResult::TokiEventResult(RoundResult result,
                                 const util::Stactor<Who, 4> &openers,
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

void TokiMountTracker::onTableEvent(const Table &table, const TE::Drawn &event)
{
    // fix the mount.
    // fixing random generator is not enough,
    // because the mount should be irrelavant from humans' drawing order.
    // (i.e. the applying order of the four mount-exits)
    // giving each exit a random seed is not enough either,
    // because we have to consider remaining tiles.
    Who who = event.who;
    const T37 &t = table.getHand(who).drawn();
    mReal.collapse(table.duringKan() ? Mount::RINSHAN : Mount::PII,
                   table.duringKan() ? mDeadPos++ : mWallPos++, t);

    // see self's draw, output to expr
    if (table.getGirl(who).getId() == Girl::Id::ONJOUJI_TOKI)
        mEvents.emplace_back(new TokiEventDrawn(t));
}

void TokiMountTracker::onTableEvent(const Table &table, const TE::Flipped &event)
{
    (void) event;

    // fix the mount
    const T37 &newIndic = table.getMount().getDrids().back();
    mReal.collapse(Mount::DORAHYOU, mDoraPos++, newIndic);

    mEvents.emplace_back(new TokiEventFlipped(newIndic));
}

void TokiMountTracker::onTableEvent(const Table &table, const TE::Discarded &event)
{
    bool riichi = mToRiichi;
    if (riichi)
        mToRiichi = false;

    mEvents.emplace_back(new TokiEventDiscarded(table.getFocusTile(), event.spin, riichi));
}

void TokiMountTracker::onTableEvent(const Table &table, const TE::RiichiCalled &event)
{
    (void) table;
    (void) event;

    mToRiichi = true;
}

void TokiMountTracker::onTableEvent(const Table &table, const TE::Barked &event)
{
    (void) table;

    mEvents.emplace_back(new TokiEventBarked(event.who, event.bark));
}

void TokiMountTracker::onTableEvent(const Table &table, const TE::RoundEnded &event)
{
    util::Stactor<util::Stactor<T37, 13>, 4> closeds;
    const auto &openers = event.openers;
    RoundResult result = event.result;
    for (Who w : openers)
        closeds.pushBack(table.getHand(w).closed().t37s13(true));

    if (result == RoundResult::RON || result == RoundResult::TSUMO) {
        bool ron = result == RoundResult::RON;
        const T37 &pick = ron ? table.getFocusTile()
                              : table.getHand(openers[0]).drawn();

        const auto &urids = table.getMount().getUrids();
        for (size_t i = 0; i < urids.size(); i++)
            mReal.collapse(Mount::URAHYOU, i, urids[i]);

        mEvents.emplace_back(new TokiEventResult(result, openers, closeds, pick, urids));
    } else { // ryuukyoku
        mEvents.emplace_back(new TokiEventResult(result, openers, closeds));
    }
}

const TokiEvents &TokiMountTracker::getEvents() const
{
    return mEvents;
}



TokiHumanSimulator::TokiHumanSimulator(const Action &firstAction,
                                       const std::array<Girl::Id, 4> &ids)
    : mFirstAction(firstAction)
{
    for (int w = 0; w < 4; w++) {
        if (ids[w] == Girl::Id::ONJOUJI_TOKI)
            mAis[w] = nullptr;
        else
            mAis[w] = Ai::create(ids[w]);
    }
}

std::array<TableDecider *, 4> TokiHumanSimulator::makeDeciders()
{
    std::array<TableDecider *, 4> res;

    using Up = std::unique_ptr<TableDecider>;
    auto toPtr = [this](Up &up) { return up ? up.get() : this; };
    std::transform(mAis.begin(), mAis.end(), res.begin(), toPtr);

    return res;
}

TableDecider::Decision TokiHumanSimulator::decide(const TableView &view)
{
    Decision decision;

    decision.action = think(view);
    if (decision.action.act() == ActCode::NOTHING)
        decision.abortTable = true;

    return decision;
}

Action TokiHumanSimulator::think(const TableView &view)
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
            os << '\n' << mOpeners[w].turnFrom(toki) << "J " << util::stringOf(mResult)
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
        os << '\n' << util::stringOf(mResult);
    }
}



bool Toki::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    (void) table;

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
        return mPredictResult;
    default:
        unreached("Toki::popUpStr");
    }
}

Girl::IrsCtrlGetter Toki::attachIrsOnDrawn(const Table &table)
{
    if (mInFuture) // forbid recursive prediction
        return nullptr;

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
    mChoices.setIrsClick(normal);
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

    TokiMountTracker mountTracker(mount, mSelf);
    Table future(table, { &mountTracker });

    TokiHumanSimulator ths(action, makeIdArray(table));
    TableTester tester(future, ths.makeDeciders());
    tester.run(true);

    if (!util::has(mRecords, action))
        mRecords.push_back(action);

    mPredictResult = mountTracker.getEvents().str(mSelf);
    popUpBy(table, PopUpMode::FV);

    return { true, &Toki::mIrsCtrl };
}

void Toki::popUpBy(const Table &table, Toki::PopUpMode mode)
{
    mPopUpMode = mode;
    table.popUp(mSelf);
}



} // namespace saki
