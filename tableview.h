#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include "who.h"
#include "hand.h"
#include "ticketfolder.h"
#include "tablefocus.h"

#include <vector>



namespace saki
{



class Table;
class Girl;

///
/// \brief A proxy accesser to prevent human or AI from knowing too
///        much information
///
class TableView
{
public:
    // TODO no need to use more complex things
    //      one enum to solve all, setViewMode(NORMAL/MOMO/MAKO/MEGAN...)
    enum class Mode { NORMAL };

    explicit TableView(const Table &table, Who viewer, Mode mode);
    TableView(const TableView &copy) = default;

    bool iCan(ActCode act) const;
    bool iCanOnlySpin() const;
    const Girl &me() const;
    const TicketFolder &myTickets() const;
    std::vector<Action> myChoices() const;
    const std::vector<T37> &mySwappables() const;
    const std::vector<T34> &myAnkanables() const;
    const std::vector<int> &myKakanables() const;
    const Hand &myHand() const;
    int myRank() const;

    int getRound() const;
    Who getDealer() const;
    const TableFocus &getFocus() const;
    const T37 &getFocusTile() const;
    const std::vector<T37> &getDrids() const;
    const RuleInfo &getRuleInfo() const;
    int getSelfWind() const;
    int getRoundWind() const;
    TileCount visibleRemain() const;

    const std::vector<T37> &getRiver(Who who) const;
    const std::vector<M37> &getBarks(Who who) const;

    bool genbutsu(Who whose, T34 t) const;
    bool riichiEstablished(Who who) const;
    bool isAllLast() const;
    bool inIppatsuCycle() const;

private:
    const Table &mTable;
    const Who mViewer;
    const Mode mMode;
};



} // namespace saki



#endif // TABLEVIEW_H


