#include "shiraitodai_teru.h"
#include "../table/princess.h"
#include "../table/table.h"
#include "../table/table_view_hand.h"
#include "../ai/ai.h"
#include "../util/misc.h"

#include <sstream>



namespace saki
{



void Teru::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) exists;
    (void) princess;
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

void Teru::onRoundEnded(const Table &table, RoundResult result,
                        const std::vector<Who> &openers, Who gunner,
                        const std::vector<Form> &fs)
{
    (void) table;
    (void) gunner;

    if (result == RoundResult::TSUMO && openers[0] == mSelf) {
        mPrevGain = fs[0].netGain();
    } else if (result == RoundResult::RON) {
        if (openers[0] == mSelf)
            mPrevGain = fs[0].netGain();
        else if (fs.size() == 2 && openers[1] == mSelf)
            mPrevGain = fs[1].netGain();
    } else {
        mPrevGain = 0;
    }
}

void Teru::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                     std::bitset<Girl::NUM_NM_SKILL> &presence,
                     const Princess &princess)
{
    (void) rand;
    (void) init;
    (void) mount;
    (void) presence; // TODO care about zim and kuro
    (void) princess;
    // TODO implement
}



} // namespace saki
