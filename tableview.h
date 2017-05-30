#ifndef SAKI_TABLEVIEW_H
#define SAKI_TABLEVIEW_H

#include "who.h"
#include "hand.h"
#include "girl.h"
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
    enum class Mode { NORMAL };

    explicit TableView(const Table &table, Who viewer, Mode mode);
    TableView(const TableView &copy) = default;

    Action mySweep() const;
    const Choices &myChoices() const;
    const Girl &me() const;
    const Hand &myHand() const;
    int myRank() const;

    int getRound() const;
    Who getDealer() const;
    const TableFocus &getFocus() const;
    const T37 &getFocusTile() const;
    const util::Stactor<T37, 5> &getDrids() const;
    const RuleInfo &getRuleInfo() const;
    int getSelfWind(Who who) const;
    int getRoundWind() const;
    TileCount visibleRemain() const;
    Who findGirl(Girl::Id id) const;

    const util::Stactor<T37, 24> &getRiver(Who who) const;
    const util::Stactor<M37, 4> &getBarks(Who who) const;

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


