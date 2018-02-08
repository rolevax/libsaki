#include "shiraitodai_takami.h"
#include "../table/princess.h"
#include "../table/table.h"
#include "../table/table_view_hand.h"
#include "../ai/ai.h"
#include "../util/misc.h"

#include <sstream>



namespace saki
{



void Takami::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                       std::bitset<Girl::NUM_NM_SKILL> &presence,
                       const Princess &princess)
{
    (void) rand;
    (void) presence;
    (void) princess;

    if (!princess.getTable().isAllLast())
        return;

    // assume seed slots has no vertical overflow
    // assume Takami::nonMonkey() is executed before any other
    // non-monkey such that initPopExact always works
    int slotCt = std::min(13, static_cast<int>(mSlots.size()));
    for (int i = 0; i < slotCt; i++) {
        T37 t = mSlots[i];
        if (mount.remainA(t) == 0)
            t = t.toInverse5();

        init.inc(mount.initPopExact(t), 1);
    }
}

void Takami::onTableEvent(const Table &table, const TableEvent &event)
{
    if (event.type() != TableEvent::Type::DISCARDED)
        return;

    if (table.getFocus().who() != mSelf)
        return;

    if (table.getRiver(mSelf).size() == 1) {
        const T37 &last = table.getFocusTile();
        // ignore if over-4
        if (std::count(mSlots.begin(), mSlots.end(), last) < 4)
            mSlots.push_back(last);
    }
}

int Takami::d3gNeed(T34 t) const
{
    if (t.suit() != Suit::Y)
        return 0;

    T37 t37(t.id34());
    int ct = std::count(mSlots.begin(), mSlots.end(), t37);
    return ct >= 3 ? 0 : 3 - ct;
}



} // namespace saki
