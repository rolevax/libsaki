#ifndef SAKI_GIRLS_UTIL_TOKI_H
#define SAKI_GIRLS_UTIL_TOKI_H

#include "table_observer.h"
#include "table_operator.h"
#include "girl.h"



namespace saki
{



class TableView;
class Mount;

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
                    const std::vector<Who> &openers,
                    const util::Stactor<util::Stactor<T37, 13>, 4> &closeds);
    TokiEventResult(RoundResult result,
                    const std::vector<Who> &openers,
                    const util::Stactor<util::Stactor<T37, 13>, 4> &closeds,
                    const T37 &pick,
                    const util::Stactor<T37, 5> &urids);
    explicit TokiEventResult(const TokiEventResult &copy) = default;
    TokiEventResult *clone() const override;
    void print(std::ostream &os, Who toki) const override;

private:
    RoundResult mResult;
    std::vector<Who> mOpeners;
    util::Stactor<util::Stactor<T37, 13>, 4> mCloseds;
    T37 mPick;
    util::Stactor<T37, 5> mUrids;
};

class TokiMountTracker : public TableObserver
{
public:
    explicit TokiMountTracker(Mount &mount, Who self);

    explicit TokiMountTracker(const TokiMountTracker &copy) = delete;
    TokiMountTracker &operator=(const TokiMountTracker &assign) = delete;

    void onDrawn(const Table &table, Who who) override;
    void onFlipped(const Table &table) override;
    void onDiscarded(const Table &table, bool spin) override;
    void onRiichiCalled(Who who) override;
    void onBarked(const Table &table, Who who, const M37 &bark, bool spin) override;
    void onRoundEnded(const Table &table, RoundResult result,
                      const std::vector<Who> &openers, Who gunner,
                      const std::vector<Form> &fs) override;

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



class TokiAutoOp : public TableOperator
{
public:
    using FourOps = std::array<std::unique_ptr<TableOperator>, 4>;
    static FourOps create(const std::array<Girl::Id, 4> &ids, const Action &firstAction);

    TokiAutoOp(Who self, const Action &firstAction);

    void onActivated(Table &table);

private:
    Action think(const TableView &view);

private:
    bool mFirst = true;
    Action mFirstAction;
};



} // namespace saki



#endif // SAKI_GIRLS_UTIL_TOKI_H


