#include "himematsu_suzu.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"



namespace saki
{



const std::array<int, 4> Suzu::POWERS { 80, 130, 230, 330 };

void Suzu::onDice(util::Rand &rand, const Table &table)
{
    if (!mExploded) {
        int remainRound = table.getRule().roundLimit - table.getRound();
        int threshold = remainRound < 4 ? 25 : 10;
        if (rand.gen(100) < threshold) {
            mExploded = true;
            table.popUp(mSelf);
        }
    }
}

void Suzu::onMonkey(std::array<Exist, 4> &exists, const Table &table)
{
    (void) table;

    if (!mExploded)
        return;

    for (Suit s : { Suit::M, Suit::P, Suit::S })
        for (int i = 0; i < 4; i++)
            exists[mSelf.index()].incMk(T34(s, 6 + i), POWERS[i]);

    for (T34 t : tiles34::Z7)
        exists[mSelf.index()].incMk(t, Z_POWER);
}

bool Suzu::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    (void) table;

    if (who != mSelf || iter > 10)
        return true;

    // no quad
    for (T34 t : tiles34::ALL34)
        if (init.closed().ct(t) == 4)
            return false;

    return true;
}

void Suzu::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || !mExploded || rinshan)
        return;

    const TileCount &closed = table.getHand(mSelf).closed();

    for (Suit s : { Suit::M, Suit::P, Suit::S }) {
        for (int i = 0; i < 4; i++) {
            T34 t(s, 6 + i);
            if (closed.ct(t) < 3)
                mount.lightA(t, POWERS[i]);
        }
    }

    for (T34 t : tiles34::Z7)
        if (closed.ct(t) < 3)
            mount.lightA(t, Z_POWER);
}

std::string Suzu::popUpStr() const
{
    return std::string("DUANG");
}



} // namespace saki
