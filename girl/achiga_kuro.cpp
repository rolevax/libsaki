#include "achiga_kuro.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"

#include <cstdlib>



namespace saki
{



void Kuro::onMonkey(std::array<Exist, 4> &exists, const Table &table)
{
    T34 dora = table.getMount().getDrids().front().dora();

    using namespace tiles37;

    int self = mSelf.index();
    if (mCd) {
        exists[self].incMk(dora, -EJECT_MK);
        exists[self].incMk(0_m, -EJECT_MK);
        exists[self].incMk(0_p, -EJECT_MK);
        exists[self].incMk(0_s, -EJECT_MK);
    } else {
        for (int w = 0; w < 4; w++) {
            int doraDelta = w == self ? 150 : -EJECT_MK;
            int akadoraDelta = w == self ? 60 : -EJECT_MK;
            exists[w].incMk(dora, doraDelta);
            exists[w].incMk(0_m, akadoraDelta);
            exists[w].incMk(0_p, akadoraDelta);
            exists[w].incMk(0_s, akadoraDelta);
        }
    }
}

bool Kuro::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    if (who != mSelf || iter > 100)
        return true;

    // drop extreme cases
    T34 drid = table.getMount().getDrids().front();
    int doraCt = drid % init;
    int akaCt = init.ctAka5();
    return (1 <= doraCt && doraCt <= 2) && (1 <= akaCt && akaCt <= 3);
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
            int turn = table.getRiver(mSelf).size();
            const Hand &hand = table.getHand(mSelf);

            for (T34 d : doras) {
                int expect = turn < 6 ? 2 : 3;
                if (hand.ct(d) < expect)
                    mount.lightA(d, d == doras.front() ? 300 : 600, rinshan);
            }

            int akaExpect = turn < 5 ? 2 : (turn < 10 ? 3 : 4);
            int akaMk = hand.ctAka5() < akaExpect ? 200 : 60;
            mount.lightA(0_m, akaMk, rinshan);
            mount.lightA(0_p, akaMk, rinshan);
            mount.lightA(0_s, akaMk, rinshan);
        } else {
            for (T34 d : doras)
                mount.lightA(d, -EJECT_MK, rinshan);

            mount.lightA(0_m, -EJECT_MK, rinshan);
            mount.lightA(0_p, -EJECT_MK, rinshan);
            mount.lightA(0_s, -EJECT_MK, rinshan);
        }
    }
}

void Kuro::onTableEvent(const Table &table, const TableEvent &event)
{
    if (event.type() != TableEvent::Type::DISCARDED)
        return;

    if (table.getFocus().who() != mSelf)
        return;

    // for equalty, 34/37 not important
    const T37 &last = table.getFocusTile();
    T37 lastIndicator(last.indicator().id34());
    const auto &indics = table.getMount().getDrids();
    if (util::has(indics, lastIndicator) || last.isAka5())
        mCd = true;
}

HrhBargainer *Kuro::onHrhBargain(const Table &table)
{
    (void) table;
    return mCd ? nullptr : this;
}

HrhBargainer::Claim Kuro::hrhBargainClaim(int plan, T34 t)
{
    T34 dora(plan);
    return t == dora ? Claim::FOUR : (t % dora ? Claim::ANY : Claim::NONE);
}

int Kuro::hrhBargainPlanCt()
{
    return 34;
}

void Kuro::onHrhBargained(int plan, Mount &mount)
{
    T34 dora(plan);
    T37 indic(dora.indicator().id34());
    if (mount.remainA(indic) == 0)
        indic = indic.toAka5();

    mount.loadB(indic, 1);

    for (T34 t : tiles34::ALL34) {
        mount.incMk(Mount::DORAHYOU, 0, t, -100, false);
        mount.incMk(Mount::DORAHYOU, 0, t, t == indic ? 100 : -100, true);
    }
}



} // namespace saki
