#include "girls_miyamori.h"
#include "table.h"
#include "tilecount.h"
#include "util.h"



namespace saki
{



void Toyone::onDice(Rand &rand, const Table &table, TicketFolder &tickets)
{
    (void) rand;
    (void) table;
    (void) tickets;

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
                for (int ti = 0; ti < 34; ti++) {
                    T34 t(ti);
                    int mk = myHand.hasEffA(t) ? 1400 : -100;
                    mount.lightA(t, mk);
                    mount.lightB(t, mk / 10);
                }
            }
        } else if (who == mFirstRiichi && table.riichiEstablished(mSelf)) {
            for (int ti = 0; ti < 34; ti++) {
                T34 t(ti);
                int mk = myHand.hasEffA(t) ? 1400 : -100;
                mount.lightA(t, mk);
                mount.lightB(t, mk / 10);
            }
        }
    }

    if (who == mSelf && myHand.barks().size() == 4) { // tomobiki
        for (int ti = 0; ti < 34; ti++) {
            T34 t(ti);
            int mk = myHand.hasEffA(t) ? 1000 : -100;
            mount.lightA(t, mk);
            mount.lightB(t, mk / 10);
        }
    }

    // push pon-material to rivals to enforce tomobiki
    if (!myHand.isMenzen() && who != mSelf) {
        std::vector<T34> myEffs = myHand.effA4();
        std::vector<T34> herEffs = table.getHand(who).effA();

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


