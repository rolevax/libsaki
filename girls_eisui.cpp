#include "girls_eisui.h"
#include "table.h"
#include "princess.h"
#include "util.h"

#include <algorithm>
#include <cassert>



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
    if (who != mSelf || rinshan || table.getSelfWind(mSelf) != 4)
        return;

    const std::vector<M37> &barks = table.getHand(mSelf).barks();
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

    if (hasE && hasN && (ctS + ctW < 5 || ctS < 3 || ctW < 3)) {
        for (int ti = 0; ti < 34; ti++) {
            T34 t(ti);
            mount.lightA(t, -50);
            mount.lightB(t, isSW(t) ? 1000 : -50);
        }
    } else if (ctE == 3) { // drag E-ankan
        mount.lightA(1_f, 500);
    } else if (ctN == 3) { // drag N-ankan
        mount.lightA(4_f, 500);
    }
}

void Hatsumi::nonMonkey(Rand &rand, TileCount &init, Mount &mount,
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
        mount.power(Mount::Exit::DORA, 0, 1_f, -1000, false);
        mount.power(Mount::Exit::DORA, 0, 4_f, -1000, false);
    }
}




void Kasumi::onDice(Rand &rand, const Table &table, TicketFolder &tickets)
{
    (void) rand;
    (void) table;

    if (mZim.able) {
        mTicketsBackup = tickets;
        tickets = TicketFolder(ActCode::IRS_CHECK);
    }
}

bool Kasumi::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who != mSelf || iter > 100)
        return true;

    return init.step() >= (iter > 50 ? 4 : 5);
}

void Kasumi::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;

    if (!mZim.on)
        return;

    for (int w = 0; w < 4; w++) {
        for (int ti = 0; ti < 34; ti++) {
            T34 t(ti);
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
    if (rinshan || !mZim.on)
        return;

    int turn = table.getRiver(mSelf).size();
    int mk = turn < 6 ? -1000 : (turn < 12 ? -30 + 10 * (turn - 6) : 0);

    for (int ti = 0; ti < 34; ti++) {
        T34 t(ti);
        // isNum && (rivals' 1-lack || self's 2-lack)
        if (t.isNum()
                && ((t.suit() == mZimSuit && who != mSelf)
                    || (t.suit() != mZimSuit && who == mSelf)))
            mount.lightA(t, mk);
    }
}

TicketFolder Kasumi::forwardAction(const Table &table, Mount &mount, const Action &action)
{
    (void) table;
    (void) mount;
    assert(action.act() == ActCode::IRS_CHECK);

    mZim.on = action.mask() & 0b1;
    if (mZim.on)
        mZim.able = false; // once on, never off

    return mTicketsBackup;
}

const IrsCheckRow &Kasumi::irsCheckRow(int index) const
{
    assert(index == 0);
    return mZim;
}

int Kasumi::irsCheckCount() const
{
    return 1;
}

void Kasumi::nonMonkey(Rand &rand, TileCount &init, Mount &mount,
                       std::bitset<Girl::NUM_NM_SKILL> &presence,
                       const Princess &princess)
{
    (void) init;
    (void) princess;

    if (!mZim.on)
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



} // namespace saki


