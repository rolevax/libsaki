#include "girls_achiga.h"
#include "table.h"
#include "princess.h"
#include "util.h"

#include <algorithm>
#include <cassert>
#include <iostream>



namespace saki
{



void Kuro::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    T34 dora = princess.getImageIndic(Princess::Indic::DORA).dora();

    using namespace tiles37;

    int self = mSelf.index();
    if (mCd) {
        exists[self].inc(dora, -EJECT_MK);
        exists[self].inc(0_m, -EJECT_MK);
        exists[self].inc(0_p, -EJECT_MK);
        exists[self].inc(0_s, -EJECT_MK);
    } else {
        for (int w = 0; w < 4; w++) {
            int doraDelta = w == self ? INIT_DRAG_MK : -EJECT_MK;
            int akadoraDelta = w == self ? INIT_DRAG_MK : -EJECT_MK;
            exists[w].inc(dora, doraDelta);
            exists[w].inc(0_m, akadoraDelta);
            exists[w].inc(0_p, akadoraDelta);
            exists[w].inc(0_s, akadoraDelta);
        }
    }
}

void Kuro::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;

    std::vector<T34> doras;
    for (const T37 &indic : mount.getDrids())
        doras.push_back(indic.dora());

    using namespace tiles37;

    if (mCd) {
        if (who != mSelf)
            return;
        for (T34 d : doras)
            mount.lightA(d, -EJECT_MK, rinshan);

        mount.lightA(0_m, -EJECT_MK, rinshan);
        mount.lightA(0_p, -EJECT_MK, rinshan);
        mount.lightA(0_s, -EJECT_MK, rinshan);
    } else {
        if (who == mSelf) {
            for (T34 d : doras)
                mount.lightA(d, DRAG_MK, rinshan);
        } else {
            for (T34 d : doras)
                mount.lightA(d, -EJECT_MK, rinshan);
        }

        mount.lightA(0_m, who == mSelf ? DRAG_MK / 3 : -EJECT_MK, rinshan);
        mount.lightA(0_p, who == mSelf ? DRAG_MK / 3 : -EJECT_MK, rinshan);
        mount.lightA(0_s, who == mSelf ? DRAG_MK / 3 : -EJECT_MK, rinshan);
    }
}

void Kuro::onDiscarded(const Table &table, Who who)
{
    if (who != mSelf)
        return;

    // for equalty, 34/37 not important
    const T37 &last = table.getFocusTile();
    T37 lastIndicator(last.indicator().id34());
    const auto &indics = table.getMount().getDrids();
    if (util::has(indics, lastIndicator) || last.isAka5())
        mCd = true;
}



using namespace tiles34;
const std::vector<T34> Yuu::LV5_TARS { 1_m, 2_m, 3_m, 4_m, 5_m, 6_m, 7_m, 8_m, 9_m, 3_y };
const std::vector<T34> Yuu::LV4_TARS { 6_p, 7_p, 9_p, 9_s };
const std::vector<T34> Yuu::LV3_TARS { 1_p, 3_p, 5_p, 1_s, 5_s, 7_s };

void Yuu::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;

    for (T34 t : LV5_TARS)
        exists[mSelf.index()].inc(t, LV5_VAL);

    for (T34 t : LV4_TARS)
        exists[mSelf.index()].inc(t, LV4_VAL);

    for (T34 t : LV3_TARS)
        exists[mSelf.index()].inc(t, LV3_VAL);
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


