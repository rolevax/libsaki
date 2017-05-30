#include "tableview.h"
#include "table.h"
#include "form_gb.h"



namespace saki
{



TableView::TableView(const Table &table, Who viewer, TableView::Mode mode)
    : mTable(table)
    , mViewer(viewer)
    , mMode(mode)
{
}

Action TableView::mySweep() const
{
    return mTable.getChoices(mViewer).sweep();
}

const Choices &TableView::myChoices() const
{
    return mTable.getChoices(mViewer);
}

const Girl &TableView::me() const
{
    return mTable.getGirl(mViewer);
}

const Hand &TableView::myHand() const
{
    return mTable.getHand(mViewer);
}

int TableView::myRank() const
{
    return mTable.getRank(mViewer);
}

int TableView::getRound() const
{
    return mTable.getRound();
}

Who TableView::getDealer() const
{
    return mTable.getDealer();
}

const TableFocus &TableView::getFocus() const
{
    return mTable.getFocus();
}

const T37 &TableView::getFocusTile() const
{
    return mTable.getFocusTile();
}

const util::Stactor<T37, 5> &TableView::getDrids() const
{
    return mTable.getMount().getDrids();
}

const RuleInfo &TableView::getRuleInfo() const
{
    return mTable.getRuleInfo();
}

int TableView::getSelfWind(Who who) const
{
    return mTable.getSelfWind(who);
}

int TableView::getRoundWind() const
{
    return mTable.getRoundWind();
}

TileCount TableView::visibleRemain() const
{
    return mTable.visibleRemain(mViewer);
}

Who TableView::findGirl(Girl::Id id) const
{
    return mTable.findGirl(id);
}

const util::Stactor<T37, 24> &TableView::getRiver(Who who) const
{
    return mTable.getRiver(who);
}

const util::Stactor<M37, 4> &TableView::getBarks(Who who) const
{
    return mTable.getHand(who).barks();
}

bool TableView::genbutsu(Who whose, T34 t) const
{
    return mTable.genbutsu(whose, t);
}

bool TableView::riichiEstablished(Who who) const
{
    return mTable.riichiEstablished(who);
}

bool TableView::isMenzen(Who who) const
{
    return mTable.getHand(who).isMenzen();
}

bool TableView::isAllLast() const
{
    return mTable.isAllLast();
}

bool TableView::inIppatsuCycle() const
{
    return mTable.inIppatsuCycle();
}



} // namespace saki


