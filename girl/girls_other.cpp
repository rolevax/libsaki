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

    if (table.getRiver(mSelf).size() < 6) {
        if (hand.step() <= 1)
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
                bool greedy = tsumoHan >= 7 && table.riichiEstablished(mSelf);
                mount.lightA(t, strong && !greedy ? 300 : -50);
            }
        } else {
            if (!tryPowerDye(hand, mount))
                power3sk(hand, mount);
        }
    }
}

void Uta::power3sk(const Hand &hand, Mount &mount)
{
    const auto parseds = hand.parse4();

    auto comp = [](const Parsed &a, const Parsed &b) {
        auto needA = a.claim3sk();
        auto needB = b.claim3sk();

        if (needA.size() < needB.size())
            return true;

        if (needA.size() == needB.size()) {
            auto isYao = [](T34 t) { return t.isYao(); };
            return util::any(needA, isYao) && util::none(needB, isYao);
        }

        return false;
    };

    auto needs = std::min_element(parseds.begin(), parseds.end(), comp)->claim3sk();
    for (T34 t : needs)
        mount.lightA(t, 100);

    const auto &drids = mount.getDrids();
    if (hand.ctAka5() + drids % hand < 3) {
        for (const T37 &t : drids)
            mount.lightA(t.dora(), 80);
        mount.lightA(T37(Suit::M, 0), 30);
        mount.lightA(T37(Suit::P, 0), 30);
        mount.lightA(T37(Suit::S, 0), 30);
    }
}

bool Uta::tryPowerDye(const Hand &hand, Mount &mount)
{
    const TileCount &closed = hand.closed();

    auto sum = [&closed](Suit s) {
        int sum = 0;
        for (int v = 1; v <= 9; v++)
            sum += closed.ct(T34(s, v));
        return sum;
    };

    std::array<Suit, 3> mps { Suit::M, Suit::P, Suit::S };
    std::array<int, 3> sums;
    std::transform(mps.begin(), mps.end(), sums.begin(), sum);

    auto it = std::max_element(sums.begin(), sums.end());
    if (*it < 9)
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

    if (turn >= 6 && s7 <= 2)
        accelerate(mount, hand, table.getRiver(mSelf), 200);
}



} // namespace saki


