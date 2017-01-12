#include "tableview.h"
#include "table.h"



namespace saki
{



TableView::TableView(const Table &table, Who viewer, TableView::Mode mode)
    : mTable(table)
    , mViewer(viewer)
    , mMode(mode)
{

}

bool TableView::iCan(ActCode act) const
{
    return mTable.getTicketFolder(mViewer).can(act);
}

bool TableView::iCanOnlySpin() const
{
    return mTable.getTicketFolder(mViewer).spinOnly();
}

const Girl &TableView::me() const
{
    return mTable.getGirl(mViewer);
}

const TicketFolder &TableView::myTickets() const
{
    return mTable.getTicketFolder(mViewer);
}

std::vector<Action> TableView::myChoices() const
{
    return mTable.getTicketFolder(mViewer).choices();
}

const std::vector<T37> &TableView::mySwappables() const
{
    return mTable.getTicketFolder(mViewer).swappables();
}

const std::vector<T34> &TableView::myAnkanables() const
{
    return mTable.getTicketFolder(mViewer).ankanables();
}

const std::vector<int> &TableView::myKakanables() const
{
    return mTable.getTicketFolder(mViewer).kakanables();
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

const std::vector<T37> &TableView::getDrids() const
{
    return mTable.getMount().getDrids();
}

const RuleInfo &TableView::getRuleInfo() const
{
    return mTable.getRuleInfo();
}

int TableView::getSelfWind() const
{
    return mTable.getSelfWind(mViewer);
}

int TableView::getRoundWind() const
{
    return mTable.getRoundWind();
}

TileCount TableView::visibleRemain() const
{
    return mTable.visibleRemain(mViewer);
}

const std::vector<T37> &TableView::getRiver(Who who) const
{
    return mTable.getRiver(who);
}

const std::vector<M37> &TableView::getBarks(Who who) const
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

bool TableView::isAllLast() const
{
    return mTable.isAllLast();
}

bool TableView::inIppatsuCycle() const
{
    return mTable.inIppatsuCycle();
}



} // namespace saki


