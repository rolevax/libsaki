#include "shiraitodai_teru.h"
#include "../table/princess.h"
#include "../table/table.h"
#include "../table/table_view_hand.h"
#include "../ai/ai.h"
#include "../util/misc.h"

#include <sstream>



namespace saki
{



void Teru::onMonkey(std::array<Exist, 4> &exists, const Table &table)
{
    (void) exists;
    (void) table;
    // TODO implement
}

void Teru::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;
    (void) mount;
    (void) rinshan;

    if (who != mSelf)
        return;

    // TODO implement
}

void Teru::onTableEvent(const Table &table, const TableEvent &event)
{
    (void) table;

    if (event.type() != TableEvent::Type::ROUND_ENDED)
        return;

    const auto &args = event.as<TableEvent::RoundEnded>();

    if (args.result == RoundResult::TSUMO && args.openers[0] == mSelf) {
        mPrevGain = args.forms[0].netGain();
    } else if (args.result == RoundResult::RON) {
        if (args.openers[0] == mSelf)
            mPrevGain = args.forms[0].netGain();
        else if (args.forms.size() == 2 && args.openers[1] == mSelf)
            mPrevGain = args.forms[1].netGain();
    } else {
        mPrevGain = 0;
    }
}



} // namespace saki
