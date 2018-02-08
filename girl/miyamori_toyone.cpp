#include "miyamori_toyone.h"
#include "../table/table.h"
#include "../util/misc.h"



namespace saki
{



void Toyone::onDice(util::Rand &rand, const Table &table)
{
    (void) rand;
    (void) table;

    mFirstRiichi = Who();
}

void Toyone::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (rinshan)
        return;

    const Hand &myHand = table.getHand(mSelf);

    if (mFirstRiichi.somebody() && mFirstRiichi != mSelf) { // senbu
        if (who == mSelf) {
            if (myHand.isMenzen() && !myHand.ready()) {
                for (T34 t : tiles34::ALL34) {
                    int mk = myHand.hasEffA(t) ? 1400 : -100;
                    mount.lightA(t, mk);
                    mount.lightB(t, mk / 10);
                }
            }
        } else if (who == mFirstRiichi && table.riichiEstablished(mSelf)) {
            for (T34 t : tiles34::ALL34) {
                int mk = myHand.hasEffA(t) ? 1400 : -100;
                mount.lightA(t, mk);
                mount.lightB(t, mk / 10);
            }
        }
    }

    if (who == mSelf && myHand.barks().size() == 4) { // tomobiki
        for (T34 t : tiles34::ALL34) {
            int mk = myHand.hasEffA(t) ? 1000 : -100;
            mount.lightA(t, mk);
            mount.lightB(t, mk / 10);
        }
    }

    // push pon-material to rivals to enforce tomobiki
    if (!myHand.isMenzen() && who != mSelf) {
        util::Stactor<T34, 34> myEffs = myHand.effA4();
        util::Stactor<T34, 34> herEffs = table.getHand(who).effA();

        for (T34 t : myEffs)
            if (myHand.canPon(t))
                if (!util::has(herEffs, t))
                    mount.lightA(t, 1000);
    }
}

void Toyone::onTableEvent(const Table &table, const TableEvent &event)
{
    (void) table;

    if (event.type() != TableEvent::Type::RIICHI_ESTABLISHED)
        return;

    if (mFirstRiichi.nobody())
        mFirstRiichi = event.as<TableEvent::RiichiEstablished>().who;
}



} // namespace saki
