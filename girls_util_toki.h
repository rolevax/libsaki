#ifndef GIRLS_UTIL_TOKI_H
#define GIRLS_UTIL_TOKI_H

#include "tableobserver.h"
#include "tableoperator.h"
#include "girl.h"



namespace saki
{



class TableView;
class Mount;

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

    const SkillExpr &getExpr() const;

private:
    Mount &mReal;
    Who mSelf;
    size_t mWallPos = 0; // wall position counter
    size_t mDeadPos = 0; // dead wall position counter
    size_t mDoraPos = 0;
    SkillExpr mExpr;
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
    bool mDiscarded;
};



} // namespace saki



#endif // GIRLS_UTIL_TOKI_H


