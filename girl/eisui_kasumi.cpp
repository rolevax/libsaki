#include "eisui_kasumi.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"



namespace saki
{



bool Kasumi::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who != mSelf || iter > 100)
        return true;

    // prevent both too low step and too many Z
    return init.step() >= 2 && (iter > 50 || init.closed().ctZ() <= 5);
}

void Kasumi::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;

    if (!mIrsCtrl.itemAt(0).on())
        return;

    for (int w = 0; w < 4; w++) {
        for (T34 t : tiles34::ALL34) {
            // isNum && (rivals' 1-lack || self's 2-lack)
            if (t.isNum()
                && ((t.suit() == mZimSuit && Who(w) != mSelf)
                    || (t.suit() != mZimSuit && Who(w) == mSelf)))
                exists[w].inc(t, -1000);

            if (t.isZ() && Who(w) == mSelf)
                exists[w].inc(t, -20);
        }
    }
}

void Kasumi::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (rinshan || !mIrsCtrl.itemAt(0).on())
        return;

    int turn = table.getRiver(mSelf).size();
    if (turn > 12)
        return;

    if (who == mSelf) {
        Suit s1 = static_cast<Suit>((static_cast<int>(mZimSuit) + 1) % 3);
        Suit s2 = static_cast<Suit>((static_cast<int>(mZimSuit) + 2) % 3);
        const Hand &hand = table.getHand(mSelf);
        int currStep = hand.step();

        for (int v = 1; v <= 9; v++) {
            mount.lightA(T34(s1, v), -1100);
            mount.lightA(T34(s2, v), -1100);

            if (currStep <= 1) {
                T34 t(mZimSuit, v);
                if (!hand.hasEffA(t))
                    mount.lightA(t, 270);
            }
        }
    } else { // rival's draw
        for (int v = 1; v <= 9; v++)
            mount.lightA(T34(mZimSuit, v), -700);
    }
}

void Kasumi::onIrsChecked(const Table &table, Mount &mount)
{
    (void) table;
    (void) mount;

    if (mIrsCtrl.itemAt(0).on())
        mIrsCtrl.setAbleAt(0, false); // once on, never off

}

void Kasumi::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                       std::bitset<Girl::NUM_NM_SKILL> &presence,
                       const Princess &princess)
{
    (void) init;
    (void) princess;

    if (!mIrsCtrl.itemAt(0).on())
        return;

    std::vector<int> ofss = { 0, 9, 18 };
    if (presence[WHITE_CLOUD])
        ofss.pop_back(); // exclude suit-S

    int door = rand.gen(ofss.size());
    mZimSuit = static_cast<Suit>(door); // ok since popped S is the last
    presence.set(ZIM_M + door); // assuming order of enum
    int begin = ofss[door];

    // break wallability of zim-tiles
    for (int i = 0; i < 9; i++)
        mount.loadB(T37(begin + i), 1);
}

Girl::IrsCtrlGetter Kasumi::attachIrsOnDice()
{
    bool able = mIrsCtrl.itemAt(0).able();
    return able ? &Kasumi::mIrsCtrl : nullptr;
}



} // namespace saki
