#include "eisui_kasumi.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"



namespace saki
{



bool Kasumi::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    (void) table;

    if (who != mSelf || iter > 100)
        return true;

    // prevent both too low step and too many Z
    return init.step() >= 2 && (iter > 50 || init.closed().ctZ() <= 5);
}

void Kasumi::onMonkey(std::array<Exist, 4> &exists, const Table &table)
{
    (void) table;

    if (!mIrsCtrl.itemAt(0).on())
        return;

    for (int w = 0; w < 4; w++) {
        for (T34 t : tiles34::ALL34) {
            // isNum && (rivals' 1-lack || self's 2-lack)
            if (t.isNum()
                && ((t.suit() == mZimSuit && Who(w) != mSelf)
                    || (t.suit() != mZimSuit && Who(w) == mSelf)))
                exists[w].incMk(t, -1000);

            if (t.isZ() && Who(w) == mSelf)
                exists[w].incMk(t, -20);
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

HrhBargainer *Kasumi::onHrhBargain(const Table &table)
{
    (void) table;
    return mIrsCtrl.itemAt(0).on() ? this : nullptr;
}

HrhBargainer::Claim Kasumi::hrhBargainClaim(int plan, T34 t)
{
    Suit suit = static_cast<Suit>(plan);
    assert(suit == Suit::M || suit == Suit::P || suit == Suit::S);
    return t.suit() == suit ? Claim::ALL : Claim::NONE;
}

int Kasumi::hrhBargainPlanCt()
{
    return 3; // three kinds of number suits
}

void Kasumi::onHrhBargained(int plan, Mount &mount)
{
    (void) mount;
    mZimSuit = static_cast<Suit>(plan);
}

Girl::IrsCtrlGetter Kasumi::attachIrsOnDice()
{
    bool able = mIrsCtrl.itemAt(0).able();
    return able ? &Kasumi::mIrsCtrl : nullptr;
}



} // namespace saki
