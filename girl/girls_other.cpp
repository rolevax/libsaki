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

    // TODO implement
    (void) table; (void) mount;
}



} // namespace saki


