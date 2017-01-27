#include "girls_miyamori.h"
#include "table.h"
#include "tilecount.h"

#include <iostream>



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

    const Hand &hand = table.getHand(mSelf);

    if (mFirstRiichi.somebody() && mFirstRiichi != mSelf) { // senbu
        if (who == mSelf) {
            if (hand.isMenzen() && !hand.ready()) {
                for (int ti = 0; ti < 34; ti++) {
                    T34 t(ti);
                    int mk = hand.hasEffA(t) ? 1000 : -100;
                    mount.lightA(t, mk);
                    mount.lightB(t, mk / 10);
                }
            }
        } else if (who == mFirstRiichi && table.riichiEstablished(mSelf)) {
            for (int ti = 0; ti < 34; ti++) {
                T34 t(ti);
                int mk = hand.hasEffA(t) ? 1000 : -100;
                mount.lightA(t, mk);
                mount.lightB(t, mk / 10);
            }
        }
    }

    if (who == mSelf && hand.barks().size() == 4) { // tomobiki
        for (int ti = 0; ti < 34; ti++) {
            T34 t(ti);
            int mk = hand.hasEffA(t) ? 1000 : -100;
            mount.lightA(t, mk);
            mount.lightB(t, mk / 10);
        }
    }
}

void Toyone::onRiichiEstablished(const Table &table, Who who)
{
    (void) table;
    if (mFirstRiichi.nobody())
        mFirstRiichi = who;
}



} // namespace saki


