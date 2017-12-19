#ifndef SAKI_TABLE_VIEW_H
#define SAKI_TABLE_VIEW_H

#include "table_focus.h"
#include "../form/hand.h"
#include "../unit/who.h"
#include "../girl/girl.h"



namespace saki
{



// A proxy accesser to prevent human or AI from knowing too
// much information, and also to impl tickets filters
class TableView
{
public:
    explicit TableView(Who self) : mSelf(self) {}
    TableView(const TableView &copy) = default;
    virtual ~TableView() = default;

    Who self() const
    {
        return mSelf;
    }

    virtual const Choices &myChoices() const = 0;
    virtual const Girl &me() const = 0;
    virtual const Hand &myHand() const = 0;
    virtual int myRank() const = 0;

    virtual int getRound() const = 0;
    virtual Who getDealer() const = 0;
    virtual const TableFocus &getFocus() const = 0;
    virtual const T37 &getFocusTile() const = 0;
    virtual const util::Stactor<T37, 5> &getDrids() const = 0;
    virtual const Rule &getRule() const = 0;
    virtual int getSelfWind(Who who) const = 0;
    virtual int getRoundWind() const = 0;
    virtual TileCount visibleRemain() const = 0;
    virtual Who findGirl(Girl::Id id) const = 0;

    virtual const River &getRiver(Who who) const = 0;
    virtual const util::Stactor<M37, 4> &getBarks(Who who) const = 0;

    virtual bool genbutsu(Who whose, T34 t) const = 0;
    virtual bool riichiEstablished(Who who) const = 0;
    virtual bool isMenzen(Who who) const = 0;
    virtual bool isAllLast() const = 0;
    virtual bool inIppatsuCycle() const = 0;

protected:
    const Who mSelf;
};



} // namespace saki



#endif // SAKI_TABLE_VIEW_H
