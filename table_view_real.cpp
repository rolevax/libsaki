#include "table_view_real.h"
#include "table.h"
#include "form_gb.h"



namespace saki
{



TableViewReal::TableViewReal(const Table &table, Who self)
    : TableView(self)
    , mTable(table)
{
}

const Choices &TableViewReal::myChoices() const
{
    return mTable.getChoices(mSelf);
}

const Girl &TableViewReal::me() const
{
    return mTable.getGirl(mSelf);
}

const Hand &TableViewReal::myHand() const
{
    return mTable.getHand(mSelf);
}

int TableViewReal::myRank() const
{
    return mTable.getRank(mSelf);
}

int TableViewReal::getRound() const
{
    return mTable.getRound();
}

Who TableViewReal::getDealer() const
{
    return mTable.getDealer();
}

const TableFocus &TableViewReal::getFocus() const
{
    return mTable.getFocus();
}

const T37 &TableViewReal::getFocusTile() const
{
    return mTable.getFocusTile();
}

const util::Stactor<T37, 5> &TableViewReal::getDrids() const
{
    return mTable.getMount().getDrids();
}

const RuleInfo &TableViewReal::getRuleInfo() const
{
    return mTable.getRuleInfo();
}

int TableViewReal::getSelfWind(Who who) const
{
    return mTable.getSelfWind(who);
}

int TableViewReal::getRoundWind() const
{
    return mTable.getRoundWind();
}

TileCount TableViewReal::visibleRemain() const
{
    return mTable.visibleRemain(mSelf);
}

Who TableViewReal::findGirl(Girl::Id id) const
{
    return mTable.findGirl(id);
}

const util::Stactor<T37, 24> &TableViewReal::getRiver(Who who) const
{
    return mTable.getRiver(who);
}

const util::Stactor<M37, 4> &TableViewReal::getBarks(Who who) const
{
    return mTable.getHand(who).barks();
}

bool TableViewReal::genbutsu(Who whose, T34 t) const
{
    return mTable.genbutsu(whose, t);
}

bool TableViewReal::riichiEstablished(Who who) const
{
    return mTable.riichiEstablished(who);
}

bool TableViewReal::isMenzen(Who who) const
{
    return mTable.getHand(who).isMenzen();
}

bool TableViewReal::isAllLast() const
{
    return mTable.isAllLast();
}

bool TableViewReal::inIppatsuCycle() const
{
    return mTable.inIppatsuCycle();
}



} // namespace saki


