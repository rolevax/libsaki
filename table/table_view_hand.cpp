#include "table_view_hand.h"



namespace saki
{



TableViewHand::TableViewHand(const Table &table, const Hand &hand, Who self)
    : TableViewReal(table, self)
    , mHand(hand)
{
    Choices::ModeDrawn mode;
    mode.swapOut = true;
    mChoices.setDrawn(mode);
}

const Choices &TableViewHand::myChoices() const
{
    return mChoices;
}

const Hand &TableViewHand::myHand() const
{
    return mHand;
}




} // namespace saki


