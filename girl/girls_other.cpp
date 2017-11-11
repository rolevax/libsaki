#include "girls_other.h"
#include "../table/table.h"



namespace saki
{



void Kazue::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    if (table.getRound() >= 4)
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 80);
    else
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), -10);
}



void Uta::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    // TODO implement
    (void) table; (void) mount;
}



void Yui::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(who);
    if (hand.barks().size() > 0)
        return;
    const int s4 = hand.step4();
    const int s7 = hand.step7();
    int mk = 350;

    if (s4 <= 0 || (s4 <= 3 && s7 > s4 + 1))
        return;
    if (s4 <= 3 && s7 > s4)
        mk = 100;
    else if (s4 == 1 && s7 <= 2)
        mk = 50;
    for (T34 t : tiles34::ALL34)
        if (hand.hasEffA7(t))
            mount.lightA(t, mk, false);

}

bool Yui::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    if (who != mSelf || iter > 100)
        return true;

    const int s4 = init.step4();
    const int s7 = init.step7();

    return s4 <= 3 || s7 <= 3 || s7 <= s4;
}



} // namespace saki


