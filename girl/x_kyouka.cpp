#include "x_kyouka.h"
#include "../table/table.h"
#include "../util/misc.h"

#include <bitset>
#include <numeric>
#include <sstream>



namespace saki
{



void Kyouka::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const TileCount &closed = table.getHand(mSelf).closed();

    for (T34 t : tiles34::ALL34) {
        int ct = closed.ct(t);
        const std::array<int, 5> deltas { 0, 40, 140, 0, 0 };
        mount.lightA(t, deltas[ct]);
    }
}



} // namespace saki
