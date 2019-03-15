#include "achiga_yuu.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"

#include <cstdlib>



namespace saki
{



using namespace tiles34;
const std::vector<T34> Yuu::LV5_TARS { 1_m, 2_m, 3_m, 4_m, 5_m, 6_m, 7_m, 8_m, 9_m, 3_y };
const std::vector<T34> Yuu::LV4_TARS { 6_p, 7_p, 9_p, 9_s };
const std::vector<T34> Yuu::LV3_TARS { 1_p, 3_p, 5_p, 1_s, 5_s, 7_s };

void Yuu::onMonkey(std::array<Exist, 4> &exists, const Table &table)
{
    (void) table;

    for (T34 t : LV5_TARS)
        exists[mSelf.uIndex()].incMk(t, LV5_VAL);

    for (T34 t : LV4_TARS)
        exists[mSelf.uIndex()].incMk(t, LV4_VAL);

    for (T34 t : LV3_TARS)
        exists[mSelf.uIndex()].incMk(t, LV3_VAL);
}

void Yuu::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;

    if (who != mSelf || rinshan)
        return;

    for (T34 t : LV5_TARS)
        mount.lightA(t, LV5_VAL);

    for (T34 t : LV4_TARS)
        mount.lightA(t, LV4_VAL);

    for (T34 t : LV3_TARS)
        mount.lightA(t, LV3_VAL);
}



} // namespace saki
