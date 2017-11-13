#include "girls_other.h"
#include "../table/table.h"



namespace saki
{



void Kazue::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    if (table.getRound() >= 4)
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 80);
    else
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), -10);
}



void Uta::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    // TODO implement
    (void) table; (void) mount;
}



void Yui::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(who);
    if (hand.barks().size() > 0)
        return;

    const int turn = table.getRiver(mSelf).size();
    const int s7 = hand.step7();

    if (turn >= 6 && s7 <= 2)
        accelerate(mount, hand, table.getRiver(mSelf), 200);
}



} // namespace saki


