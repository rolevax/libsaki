#include "x_rio.h"
#include "../table/table.h"
#include "../util/misc.h"



namespace saki
{



void Rio::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) rinshan;

    if (who != mSelf)
        return;

    const TableEnv &env = table.getEnv();
    int hour24 = env.hour24();

    if (5 <= hour24 && hour24 <= 9)
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 130);
}



} // namespace saki
