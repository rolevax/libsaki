#include "kiyosumi_nodoka.h"
#include "../table/table.h"
#include "../table/princess.h"



namespace saki
{



void Nodoka::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(mSelf);
    int step4 = hand.step4();
    int step7 = hand.step7();
    int mk = step4 == 1 ? 100 : 200;
    if (step4 == 1 || step7 == 2 || step7 == 1)
        for (T34 t : hand.effA4())
            mount.lightA(t, mk);
}



} // namespace saki
