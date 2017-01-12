#include "girls_other.h"
#include "table.h"



namespace saki
{



void Kazue::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    if (table.getRound() >= 4)
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 70);
    else
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), -10);
}



} // namespace saki


