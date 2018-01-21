#include "girls_eisui.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"



namespace saki
{



bool Hatsumi::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    if (who == mSelf || iter > 100)
        return true;

    if (princess.getTable().getSelfWind(mSelf) == 4) { // north seat
        // prevent rivals having E/N pairs
        using namespace tiles34;
        int ctE = init.closed().ct(1_f);
        int ctN = init.closed().ct(4_f);
        return ctE <= 1 && ctN <= 1;
    }

    return true;
}

void Hatsumi::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (rinshan || table.getSelfWind(mSelf) != 4)
        return;

    if (who != mSelf) {
        const Hand &hand = table.getHand(who);
        for (int v = 1; v <= 4; v++) {
            T34 t(Suit::F, v);
            // prevent rivals from having F-pairs
            // let them to have floating useless F
            mount.lightA(t, hand.ct(t) >= 1 ? -50 : (table.riichiEstablished(who) ? 500 : 80));
        }

        return;
    }

    const auto &barks = table.getHand(mSelf).barks();
    const TileCount &closed = table.getHand(mSelf).closed();
    if (barks.size() < 2)
        return;

    using namespace tiles34;
    auto checkE = [](const M37 &m) { return m[0] == 1_f; };
    auto checkN = [](const M37 &m) { return m[0] == 4_f; };
    auto isSW = [](T34 t) { return t == 2_f || t == 3_f; };

    bool hasE = util::any(barks, checkE);
    bool hasN = util::any(barks, checkN);
    int ctE = closed.ct(1_f);
    int ctS = closed.ct(2_f);
    int ctW = closed.ct(3_f);
    int ctN = closed.ct(4_f);

    if (hasE && hasN && (std::min(ctS, 3) + std::min(ctW, 3) < 5)) {
        for (T34 t : tiles34::ALL34) {
            mount.lightA(t, -50);
            mount.lightB(t, isSW(t) ? 1000 : -50);
        }
    } else if (ctE == 3) { // drag E-ankan
        mount.lightA(1_f, 500);
    } else if (ctN == 3) { // drag N-ankan
        mount.lightA(4_f, 500);
    }
}

void Hatsumi::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                        std::bitset<Girl::NUM_NM_SKILL> &presence,
                        const Princess &princess)
{
    (void) rand;
    (void) presence;

    if (princess.getTable().getSelfWind(mSelf) == 4) { // north seat
        using namespace tiles37;
        // seize E/N pair
        if (mount.remainA(1_f) >= 2) {
            init.inc(mount.initPopExact(1_f), 1);
            init.inc(mount.initPopExact(1_f), 1);
        }

        if (mount.remainA(4_f) >= 2) {
            init.inc(mount.initPopExact(4_f), 1);
            init.inc(mount.initPopExact(4_f), 1);
        }

        // reserve a triplet of S/W
        mount.loadB(2_f, 3);
        mount.loadB(3_f, 3);

        // prevent E/N to be dora
        mount.power(Mount::Exit::DORAHYOU, 0, 1_f, -1000, false);
        mount.power(Mount::Exit::DORAHYOU, 0, 4_f, -1000, false);
    }
}



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
