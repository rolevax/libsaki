#include "girls_miyamori.h"
#include "table.h"
#include "tile_count.h"
#include "util.h"



namespace saki
{



void Toyone::onDice(Rand &rand, const Table &table, Choices &choices)
{
    (void) rand;
    (void) table;
    (void) choices;

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

void Toyone::onRiichiEstablished(const Table &table, Who who)
{
    (void) table;
    if (mFirstRiichi.nobody())
        mFirstRiichi = who;
}



} // namespace saki


