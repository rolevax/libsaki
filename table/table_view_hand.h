#ifndef SAKI_TABLE_VIEW_HAND_H
#define SAKI_TABLE_VIEW_HAND_H

#include "table_view_real.h"



namespace saki
{



class TableViewHand : public TableViewReal
{
public:
    explicit TableViewHand(const Table &table, const Hand &hand, Who self);
    TableViewHand(const TableViewHand &copy) = default;

    const Choices &myChoices() const override;
    const Hand &myHand() const override;

private:
    const Hand &mHand;
    Choices mChoices;
};



} // namespace saki



#endif // SAKI_TABLE_VIEW_HAND_H


