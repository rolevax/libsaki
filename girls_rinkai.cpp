#include "girls_rinkai.h"
#include "table.h"
#include "util.h"



namespace saki
{



void Huiyu::skill(Mount &mount, const Hand &hand)
{
    TileCount total(hand.closed()); // copy
    for (const M37 &m : hand.barks())
        for (const T37 &t : m.tiles())
            total.inc(t, 1);

//    twist(mount, total);
//    tbd(mount, total);
//    qdqzqx(mount, total);
    yssbg(mount, total);
}

void Huiyu::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    skill(mount, table.getHand(mSelf));
}

void Huiyu::twist(Mount &mount, const TileCount &total)
{
    int maxMatch = 7; // trigger boundary
    std::vector<SssbgSet> maxSets;

    auto updateSet = [&](const SssbgSet &set) {
        std::array<T34, 9> tars = makeSssbgTars(set);
        auto aux = [&](int s, T34 t) { return s + (total.ct(t) > 0); };
        int sumCt = std::accumulate(tars.begin(), tars.end(), 0, aux);

        if (sumCt > maxMatch) {
            maxMatch = sumCt;
            maxSets.clear();
        }

        if (sumCt == maxMatch)
            maxSets.push_back(set);
    };

    for (int ofs = 0; ofs < 6; ofs++) // hualong
        updateSet(SssbgSet(3, ofs, 1));
    for (int ofs = 0; ofs < 6; ofs++) // 2-step sansesanbugao
        for (int v = 1; v <= 3; v++)
            updateSet(SssbgSet(2, ofs, v));
    for (int ofs = 0; ofs < 6; ofs++) // 1-step sansesanbugao
        for (int v = 1; v <= 5; v++)
            updateSet(SssbgSet(1, ofs, v));

    if (!maxSets.empty() && maxMatch < 9) {
        std::array<T34, 9> tars = makeSssbgTars(maxSets[0]); // just first
        for (T34 t : tars)
            if (total.ct(t) == 0)
                mount.lightA(t, 500);
    }
}

void Huiyu::yssbg(Mount &mount, const TileCount &total)
{
    const int BOUND = 6;
    int maxMatch = BOUND;
    T34 maxHead;
    int maxStep;

    for (Suit s : { Suit::M, Suit::P, Suit::S }) {
        for (int v = 1; v <= 3; v++) { // 2-step
            std::array<T34, 5> need1s {
                T34(s, v), T34(s, v + 1), T34(s, v + 3), T34(s, v + 5), T34(s, v + 6)
            };
            std::array<T34, 2> need2s { T34(s, v + 2), T34(s, v + 4) };
            auto aux1 = [&](int s, T34 t) { return s + (total.ct(t) > 0); };
            auto aux2 = [&](int s, T34 t) { return s + std::min(total.ct(t), 2); };
            int match1 = std::accumulate(need1s.begin(), need1s.end(), 0, aux1);
            int match2 = std::accumulate(need2s.begin(), need2s.end(), 0, aux2);
            int match = match1 + match2;
            if (match > maxMatch) {
                maxMatch = match;
                maxHead = need1s[0];
                maxStep = 2;
            }
        }

        for (int v = 1; v <= 5; v++) { // 1-step
            T34 t1(s, v);
            T34 t2(s, v + 1);
            T34 t3(s, v + 2);
            T34 t4(s, v + 3);
            T34 t5(s, v + 4);
            int match1 = std::min(total.ct(t1), 1) + std::min(total.ct(t5), 1);
            int match2 = std::min(total.ct(t2), 2) + std::min(total.ct(t4), 2);
            int match3 = std::min(total.ct(t3), 3);
            int match = match1 + match2 + match3;
            if (match > maxMatch) {
                maxMatch = match;
                maxHead = t1;
                maxStep = 1;
            }
        }
    }

util::p("ha?");
    if (BOUND < maxMatch && maxMatch < 9) {
        Suit s = maxHead.suit();
        int v = maxHead.val();
        if (maxStep == 2) {
util::p("triggered 2-step", maxHead);
            std::array<T34, 5> need1s {
                T34(s, v), T34(s, v + 1), T34(s, v + 3), T34(s, v + 5), T34(s, v + 6)
            };
            std::array<T34, 2> need2s { T34(s, v + 2), T34(s, v + 4) };
            for (T34 t : need1s)
                if (total.ct(t) < 1)
                    mount.lightA(t, 500);
            for (T34 t : need2s)
                if (total.ct(t) < 2)
                    mount.lightA(t, 500);
        } else if (maxStep == 1) {
util::p("triggered 1-step", maxHead);
            T34 t1(s, v);
            T34 t2(s, v + 1);
            T34 t3(s, v + 2);
            T34 t4(s, v + 3);
            T34 t5(s, v + 4);
            if (total.ct(t1) < 1)
                mount.lightA(t1, 500);
            if (total.ct(t2) < 2)
                mount.lightA(t2, 500);
            if (total.ct(t3) < 3)
                mount.lightA(t3, 500);
            if (total.ct(t4) < 2)
                mount.lightA(t4, 500);
            if (total.ct(t5) < 1)
                mount.lightA(t5, 500);
        }
    }
}

void Huiyu::tbd(Mount &mount, const TileCount &total)
{
    using namespace tiles34;

    const std::vector<T34> tumbler {
        1_p, 2_p, 3_p, 4_p, 5_p, 8_p, 9_p, 2_s, 4_s, 5_s, 6_s, 8_s, 9_s, 1_y
    };

    if (total.ct(tumbler) >= 10)
        for (T34 t : tumbler)
            mount.lightA(t, 500);
}

void Huiyu::qdqzqx(Mount &mount, const TileCount &total)
{
    int big = 0;
    int middle = 0;
    int small = 0;
    for (Suit s : { Suit::M, Suit::P, Suit::S }) {
        for (int v = 1; v <= 3; v++) {
            big += total.ct(T34(s, v + 6));
            middle += total.ct(T34(s, v + 3));
            small += total.ct(T34(s, v));
        }
    }

    if (big >= 10)
        for (Suit s : { Suit::M, Suit::P, Suit::S })
            for (int v = 7; v <= 9; v++)
                mount.lightA(T34(s, v), 500);
    if (middle >= 10)
        for (Suit s : { Suit::M, Suit::P, Suit::S })
            for (int v = 4; v <= 6; v++)
                mount.lightA(T34(s, v), 500);
    if (small >= 10)
        for (Suit s : { Suit::M, Suit::P, Suit::S })
            for (int v = 1; v <= 3; v++)
                mount.lightA(T34(s, v), 500);

    if (big < 10 && middle < 10 && small < 10) {
        int gt = 0;
        int lt = 0;

        for (int ti = 0; ti < 27; ti++) {
            T34 t(ti);
            int ct = total.ct(t);
            if (t.val() > 5)
                gt += ct;
            if (t.val() < 5)
                lt += ct;
        }

        if (gt >= 10)
            for (Suit s : { Suit::M, Suit::P, Suit::S })
                for (int v = 6; v <= 9; v++)
                    mount.lightA(T34(s, v), 500);
        if (lt >= 10)
            for (Suit s : { Suit::M, Suit::P, Suit::S })
                for (int v = 1; v <= 4; v++)
                    mount.lightA(T34(s, v), 500);
    }
}

const std::array<std::array<int, 3>, 6> Huiyu::SSSBG_OFSS {
    std::array<int, 3> { 0, 1, 2 },
    std::array<int, 3> { 0, 2, 1 },
    std::array<int, 3> { 1, 0, 2 },
    std::array<int, 3> { 1, 2, 0 },
    std::array<int, 3> { 2, 0, 1 },
    std::array<int, 3> { 2, 1, 0 }
};

std::array<T34, 9> Huiyu::makeSssbgTars(const SssbgSet &set)
{
    T34 m(Suit::M, set.v + set.jump * SSSBG_OFSS[set.ofs][0]);
    T34 p(Suit::P, set.v + set.jump * SSSBG_OFSS[set.ofs][1]);
    T34 s(Suit::S, set.v + set.jump * SSSBG_OFSS[set.ofs][2]);
    std::array<T34, 9> tars {
        m, m.next(), m.nnext(), p, p.next(), p.nnext(), s, s.next(), s.nnext()
    };
    return tars;
}



}


