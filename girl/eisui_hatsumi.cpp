#include "eisui_hatsumi.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"



namespace saki
{



bool Hatsumi::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    if (who == mSelf || iter > 100)
        return true;

    if (table.getSelfWind(mSelf) == 4) { // north seat
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

std::optional<HrhInitFix> Hatsumi::onHrhRaid(const Table &table)
{
    if (table.getSelfWind(mSelf) != 4) // north seat
        return std::nullopt;

    using namespace tiles37;

    HrhInitFix fix;

    fix.priority = HrhInitFix::Priority::LOW;

    fix.targets.emplaceBack(1_f);
    fix.targets.emplaceBack(1_f);
    fix.targets.emplaceBack(4_f);
    fix.targets.emplaceBack(4_f);

    fix.loads.emplaceBack(2_f, 3);
    fix.loads.emplaceBack(3_f, 3);

    return fix;
}

HrhBargainer *Hatsumi::onHrhBargain(const Table &table)
{
    return table.getSelfWind(mSelf) == 4 ? this : nullptr;
}

HrhBargainer::Claim Hatsumi::hrhBargainClaim(int plan, T34 t)
{
    (void) plan;
    using namespace tiles34;
    return t == 1_f || t == 4_f ? Claim::ANY : Claim::NONE;
}

int Hatsumi::hrhBargainPlanCt()
{
    return 1;
}

void Hatsumi::onHrhBargained(int plan, Mount &mount)
{
    (void) plan;
    (void) mount;
    // do nothing
}



} // namespace saki
