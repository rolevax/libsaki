#ifndef SAKI_TABLEVIEW_H
#define SAKI_TABLEVIEW_H

#include "who.h"
#include "hand.h"
#include "girl.h"
#include "ticketfolder.h"
#include "tablefocus.h"

#include <vector>



namespace saki
{



class Table;
class Girl;

///
/// \brief A proxy accesser to prevent human or AI from knowing too
///        much information, and also to impl tickets filters
///
class TableView
{
public:
    // not using inheritance in Girl since modes may be unions of skills
    // using enum type is temporal, should use bitset, set by Girl's virtual method
    enum class Mode { NORMAL, HUIYU_LIMITED };

    explicit TableView(const Table &table, Who viewer, Mode mode);
    TableView(const TableView &copy) = default;

    bool iCan(ActCode act) const;
    bool iCanOnlySpin() const;
    bool iForwardAny() const;
    bool iForwardAll() const;
    std::vector<Action> myChoices() const;
    Action mySweep() const;
    const std::vector<T37> &mySwappables() const;
    const std::vector<T34> &myAnkanables() const;
    const std::vector<int> &myKakanables() const;
    const Girl &me() const;
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
    Who findGirl(Girl::Id id) const;

    const std::vector<T37> &getRiver(Who who) const;
    const std::vector<M37> &getBarks(Who who) const;

    bool genbutsu(Who whose, T34 t) const;
    bool riichiEstablished(Who who) const;
    bool isMenzen(Who who) const;
    bool isAllLast() const;
    bool inIppatsuCycle() const;

private:
    const Table &mTable;
    const Who mViewer;
    const Mode mMode;
};



} // namespace saki



#endif // SAKI_TABLEVIEW_H


