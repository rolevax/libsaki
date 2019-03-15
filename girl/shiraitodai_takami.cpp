#include "shiraitodai_takami.h"
#include "../table/princess.h"
#include "../table/table.h"
#include "../table/table_view_hand.h"
#include "../ai/ai.h"
#include "../util/misc.h"

#include <sstream>



namespace saki
{



std::optional<HrhInitFix> Takami::onHrhRaid(const Table &table)
{
    if (!table.isAllLast())
        return std::nullopt;

    Mount mount(table.getMount()); // copy

    HrhInitFix fix;
    for (const T37 &seed : mSlots) {
        // use black if no red, use red if no black
        T37 bud = mount.remainA(seed) > 0 ? seed : seed.toInverse5();
        mount.initPopExact(bud);
        fix.targets.emplaceBack(bud);
    }

    fix.priority = HrhInitFix::Priority::HIGH;

    return fix;
}

void Takami::onTableEvent(const Table &table, const TableEvent &event)
{
    if (event.type() != TableEvent::Type::DISCARDED)
        return;

    if (table.getFocus().who() != mSelf || mSlots.full())
        return;

    if (table.getRiver(mSelf).size() == 1) {
        const T37 &last = table.getFocusTile();
        // ignore if over-4
        if (std::count(mSlots.begin(), mSlots.end(), last) < 4)
            mSlots.pushBack(last);
    }
}

int Takami::d3gNeed(T34 t) const
{
    if (t.suit() != Suit::Y)
        return 0;

    T37 t37(t.id34());
    int ct = static_cast<int>(std::count(mSlots.begin(), mSlots.end(), t37));
    return ct >= 3 ? 0 : 3 - ct;
}



} // namespace saki
