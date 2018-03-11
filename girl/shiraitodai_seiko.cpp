#include "shiraitodai_seiko.h"
#include "../table/princess.h"
#include "../table/table.h"
#include "../table/table_view_hand.h"
#include "../ai/ai.h"
#include "../util/misc.h"

#include <sstream>



namespace saki
{



bool Seiko::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    (void) table;

    if (who != mSelf || iter > 100)
        return true;

    // no triplet
    for (T34 t : tiles34::ALL34)
        if (init.closed().ct(t) >= 3)
            return false;

    int yaoPairCt = std::count_if(tiles34::YAO13.begin(), tiles34::YAO13.end(),
                                  [&init](T34 t) { return init.closed().ct(t) == 2; });
    return yaoPairCt >= 2;
}

void Seiko::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(mSelf);
    const TileCount &closed = hand.closed();
    const auto &barks = hand.barks();

    // *INDENT-OFF*
    const auto isFish = [](const M37 &m) {
        return m.type() == M37::Type::PON
                || m.type() == M37::Type::DAIMINKAN
                || m.type() == M37::Type::KAKAN;
    };
    // *INDENT-ON*
    int fishCt = std::count_if(barks.begin(), barks.end(), isFish);

    if (fishCt >= 3) {
        accelerate(mount, hand, table.getRiver(mSelf), ACCEL_MK);
    } else if (!hand.ready()) {
        int needPair = 4 - fishCt;
        for (T34 t : tiles34::ALL34) {
            if (closed.ct(t) == 2) {
                needPair--;
                mount.lightA(t, -PAIR_MK);
            }
        }

        if (needPair > 0) {
            using namespace tiles34;
            for (T34 t : YAO13)
                if (closed.ct(t) == 1)
                    mount.lightA(t, PAIR_MK);

            for (T34 t : { 2_m, 8_m, 2_p, 8_p, 2_s, 8_s })
                if (closed.ct(t) == 1)
                    mount.lightA(t, PAIR_MK);
        }
    }
}



} // namespace saki
