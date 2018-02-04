#include "x_yui.h"
#include "../table/table.h"
#include "../util/misc.h"



namespace saki
{



void Yui::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(who);
    if (hand.barks().size() > 0)
        return;

    const int turn = table.getRiver(mSelf).size();
    const int s7 = hand.step7();

    // *INDENT-OFF*
    auto morePairs = [&hand, &mount](int mk) {
        for (T34 t : tiles34::ALL34)
            if (hand.closed().ct(t) == 1)
                mount.lightA(t, mk);
    };
    // *INDENT-ON*

    if (turn < 6 && s7 >= 3)
        morePairs(70);

    if (turn >= 6 && s7 <= 2) {
        accelerate(mount, hand, table.getRiver(mSelf), 100);
        morePairs(100);
    }
}



} // namespace saki
