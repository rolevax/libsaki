#include "girls_other.h"
#include "../table/table.h"
#include "../util/misc.h"



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

    const Hand &hand = table.getHand(mSelf);
    if (!hand.isMenzen())
        return;

    if (table.getRiver(mSelf).size() < 5 && hand.step() <= 1) {
        for (T34 t : hand.effA())
            mount.lightA(t, -40); // slow down

    } else {
        if (hand.ready()) {
            const FormCtx &ctx = table.getFormCtx(mSelf);
            const Rule &rule = table.getRule();
            const auto &drids = mount.getDrids();

            for (T34 t : hand.effA()) {
                Form f(hand, T37(t.id34()), ctx, rule, drids);
                int ronHan = f.han();
                int tsumoHan = ronHan + 1;
                bool strong = tsumoHan >= 6;
                bool greedy = ronHan >= 7 && table.riichiEstablished(mSelf);
                mount.lightA(t, strong && !greedy ? 400 : -50);
            }
        } else {
            const River &river = table.getRiver(mSelf);
            if (!tryPowerDye(hand, mount))
                power3sk(hand, river, mount);
        }
    }
}

void Uta::power3sk(const Hand &hand, const River &river, Mount &mount)
{
    const auto parseds = hand.parse4();

    // *INDENT-OFF*
    auto inRiver = [&river](T34 t) {
        return util::any(river, [t](const T37 &r) { return t == r; });
    };

    auto comp = [&inRiver](const Parsed &a, const Parsed &b) {
        auto needA = a.claim3sk();
        auto needB = b.claim3sk();

        bool hateA = util::any(needA, inRiver);
        bool hateB = util::any(needB, inRiver);
        if (hateA != hateB)
            return hateB;

        if (needA.size() < needB.size())
            return true;

        if (needA.size() == needB.size()) {
            auto isYao = [](T34 t) { return t.isYao(); };
            return util::any(needA, isYao) && util::none(needB, isYao);
        }

        return false;
    };
    // *INDENT-ON*

    auto needs = std::min_element(parseds.begin(), parseds.end(), comp)->claim3sk();
    for (T34 t : needs)
        if (util::none(river, [t](const T37 &r) { return t == r; }))
            mount.lightA(t, 500);

    if (needs.empty() && river.size() >= 9)
        accelerate(mount, hand, river, 100);

    const auto &drids = mount.getDrids();
    if (hand.ctAka5() + drids % hand < 3)
        for (const T37 &ind : drids)
            if (util::none(river, [&ind](const T37 &r) { return ind % r; }))
                mount.lightA(ind.dora(), 100);
}

bool Uta::tryPowerDye(const Hand &hand, Mount &mount)
{
    const TileCount &closed = hand.closed();

    // *INDENT-OFF*
    auto sum = [&closed](Suit s) {
        int sum = 0;
        for (int v = 1; v <= 9; v++)
            sum += closed.ct(T34(s, v));

        return sum;
    };
    // *INDENT-ON*

    std::array<Suit, 3> mps { Suit::M, Suit::P, Suit::S };
    std::array<int, 3> sums;
    std::transform(mps.begin(), mps.end(), sums.begin(), sum);

    auto it = std::max_element(sums.begin(), sums.end());
    if (*it + closed.ctZ() < 9)
        return false;

    Suit s = mps[it - sums.begin()];
    for (int v = 1; v <= 9; v++) {
        T34 t(s, v);
        int doraVal = mount.getDrids() % t;
        mount.lightA(t, 300 + doraVal * 100);
    }

    return true;
}



void Rio::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) rinshan;

    if (who != mSelf)
        return;

    const TableEnv &env = table.getEnv();
    int hour24 = env.hour24();

    if (5 <= hour24 && hour24 <= 9)
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 130);
}



void Yui::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(who);
    if (hand.barks().size() > 0)
        return;

    const int turn = table.getRiver(mSelf).size();
    const int s7 = hand.step7();

    // *INDENT-OFF*
    auto morePairs = [&hand, &mount](int mk) {
        for (T34 t : tiles34::ALL34)
            if (hand.closed().ct(t) == 1)
                mount.lightA(t, mk);
    };
    // *INDENT-ON*

    if (turn < 6 && s7 >= 3)
        morePairs(70);

    if (turn >= 6 && s7 <= 2) {
        accelerate(mount, hand, table.getRiver(mSelf), 100);
        morePairs(100);
    }
}
} // namespace saki
