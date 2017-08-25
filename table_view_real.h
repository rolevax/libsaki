#ifndef SAKI_TABLE_VIEW_REAL_H
#define SAKI_TABLE_VIEW_REAL_H

#include "table_view.h"



namespace saki
{



class Table;
class Girl;



class TableViewReal : public TableView
{
public:
    explicit TableViewReal(const Table &table, Who self);
    TableViewReal(const TableViewReal &copy) = default;

    const Choices &myChoices() const override;
    const Girl &me() const override;
    const Hand &myHand() const override;
    int myRank() const override;

    int getRound() const override;
    Who getDealer() const override;
    const TableFocus &getFocus() const override;
    const T37 &getFocusTile() const override;
    const util::Stactor<T37, 5> &getDrids() const override;
    const RuleInfo &getRuleInfo() const override;
    int getSelfWind(Who who) const override;
    int getRoundWind() const override;
    TileCount visibleRemain() const override;
    Who findGirl(Girl::Id id) const override;

    const util::Stactor<T37, 24> &getRiver(Who who) const override;
    const util::Stactor<M37, 4> &getBarks(Who who) const override;

    bool genbutsu(Who whose, T34 t) const override;
    bool riichiEstablished(Who who) const override;
    bool isMenzen(Who who) const override;
    bool isAllLast() const override;
    bool inIppatsuCycle() const override;

private:
    const Table &mTable;
};



} // namespace saki



#endif // SAKI_TABLE_VIEW_REAL_H


