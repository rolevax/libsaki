#include "girls_rinkai.h"
#include "form_gb.h"
#include "table.h"
#include "util.h"



namespace saki
{



void Huiyu::skill(Mount &mount, const Hand &hand, const PointInfo &info)
{
    if (hand.ready()) {
        Hand copy(hand);
        bool has = false;

        for (T34 t : hand.effA()) {
            copy.draw(T37(t.id34()));
            FormGb f(copy, info, false);
            if (f.fan() >= 8) {
                has = true;
                mount.lightA(t, 300);
            }
            copy.spinOut();
        }

        if (has)
            return;
    }

    TileCount total(hand.closed()); // copy
    for (const M37 &m : hand.barks())
        for (const T37 &t : m.tiles())
            total.inc(t, 1);

    if (expand(mount, total)) // expand done, drag eff
        for (T34 t : hand.effA())
            mount.lightA(t, 200);
}

void Huiyu::onActivate(const Table &table, Choices &choices)
{
    auto filterDrawn = [&]() {
        Choices::ModeDrawn drawn = choices.drawn();

        if (drawn.tsumo) {
            const Hand &hand = table.getHand(mSelf);
            bool juezhang = table.riverRemain(hand.drawn()) == 0;
            FormGb f(hand, table.getPointInfo(mSelf), juezhang);
            if (f.fan() < 8)
                drawn.tsumo = false;
        }

        drawn.spinRiichi = false;
        drawn.swapRiichis.clear();

        choices.setDrawn(drawn);
    };

    auto filterBark = [&]() {
        const T37 &pick = table.getFocusTile();
        bool juezhang = table.riverRemain(pick) == 0;
        FormGb f(table.getHand(mSelf), pick, table.getPointInfo(mSelf), juezhang);
        if (f.fan() < 8) {
            Choices::ModeBark bark = choices.bark();
            if (bark.ron) {
                bark.ron = false;
                choices.setBark(bark);
            }
        }
    };

    switch (choices.mode()) {
    case Choices::Mode::DRAWN:
        filterDrawn();
        break;
    case Choices::Mode::BARK:
        filterBark();
        break;
    default:
        break;
    }
}

void Huiyu::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    skill(mount, table.getHand(mSelf), table.getPointInfo(mSelf));
}

bool Huiyu::expand(Mount &mount, const TileCount &total)
{
    std::bitset<34> minReqs, tempReqs;
    int minDist = 14, tempDist;
    auto update = [&](int (*f)(std::bitset<34> &reqs, const TileCount &total), int triggerDist) {
        tempReqs.reset();
        tempDist = f(tempReqs, total);
        if (tempDist <= triggerDist && tempDist < minDist) {
            minDist = tempDist;
            minReqs = tempReqs;
        }
    };

    update(yssbg, 3);
    update(colors, 2);
    update(sssjg, 4);
    update(tbd, 6);
    update(qdqzqx, 6);
    update(gtlt5, 6);

    assert(minDist >= 0);
    if (0 < minDist && minDist < 14)
        for (T34 t : tiles34::ALL34)
            mount.lightA(t, minReqs.test(t.id34()) ? 100 : -10);

    return minDist == 0;
}

int Huiyu::yssbg(std::bitset<34> &reqs, const TileCount &total)
{
    int maxMatch = 0;
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

        int matchQl = 0;
        for (int v = 1; v <= 9; v++)
            if (total.ct(T34(s, v)))
                matchQl++;
        if (matchQl > maxMatch) {
            maxMatch = matchQl;
            maxHead = T34(s, 1);
            maxStep = 3;
        }
    }

    if (0 < maxMatch && maxMatch < 9) {
        Suit s = maxHead.suit();
        int v = maxHead.val();
        if (maxStep == 3) {
            for (int v = 1; v <= 9; v++)
                if (total.ct(T34(s, v)) < 1)
                    reqs.set(T34(s, v).id34());
        } else if (maxStep == 2) {
            std::array<T34, 5> need1s {
                T34(s, v), T34(s, v + 1), T34(s, v + 3), T34(s, v + 5), T34(s, v + 6)
            };
            std::array<T34, 2> need2s { T34(s, v + 2), T34(s, v + 4) };
            for (T34 t : need1s)
                if (total.ct(t) < 1)
                    reqs.set(t.id34());
            for (T34 t : need2s)
                if (total.ct(t) < 2)
                    reqs.set(t.id34());
        } else if (maxStep == 1) {
            T34 t1(s, v);
            T34 t2(s, v + 1);
            T34 t3(s, v + 2);
            T34 t4(s, v + 3);
            T34 t5(s, v + 4);
            if (total.ct(t1) < 1)
                reqs.set(t1.id34());
            if (total.ct(t2) < 2)
                reqs.set(t2.id34());
            if (total.ct(t3) < 3)
                reqs.set(t3.id34());
            if (total.ct(t4) < 2)
                reqs.set(t4.id34());
            if (total.ct(t5) < 1)
                reqs.set(t5.id34());
        }
    }

    return 9 - maxMatch;
}

int Huiyu::colors(std::bitset<34> &reqs, const TileCount &total)
{
    int maxMatch = 0;
    std::bitset<34> res;

    auto hasOne = [&](T34 t) { return total.ct(t) > 0; };
    auto update = [&](T34 m, T34 p, T34 s) {
        std::array<T34, 9> tars {
            m, m.next(), m.nnext(),
            p, p.next(), p.nnext(),
            s, s.next(), s.nnext()
        };
        auto aux = [&](int s, T34 t) { return s + hasOne(t); };
        int match = std::accumulate(tars.begin(), tars.end(), 0, aux);
        if (match > maxMatch) {
            maxMatch = match;
            res.reset();
            for (T34 t : tars)
                if (!hasOne(t))
                    res.set(t.id34());
        }
    };

    std::array<std::array<int, 3>, 6> mpsVals {
        std::array<int, 3> { 1, 4, 7 },
        std::array<int, 3> { 1, 7, 4 },
        std::array<int, 3> { 4, 1, 7 },
        std::array<int, 3> { 4, 7, 1 },
        std::array<int, 3> { 7, 1, 4 },
        std::array<int, 3> { 7, 4, 1 }
    };

    for (const auto &vals : mpsVals) {
        T34 m(Suit::M, vals[0]);
        T34 p(Suit::P, vals[1]);
        T34 s(Suit::S, vals[2]);
        update(m, p, s);
    }

    for (int v = 1; v <= 7; v++) {
        T34 m(Suit::M, v);
        T34 p(Suit::P, v);
        T34 s(Suit::S, v);
        update(m, p, s);
    }

    reqs |= res;
    return 9 - maxMatch;
}

int Huiyu::sssjg(std::bitset<34> &reqs, const TileCount &total)
{
    int maxMatch = 0;
    std::bitset<34> maxReqs;

    std::array<std::array<int, 3>, 6> mpsVals {
        std::array<int, 3> { 1, 2, 3 },
        std::array<int, 3> { 1, 3, 2 },
        std::array<int, 3> { 2, 1, 3 },
        std::array<int, 3> { 2, 3, 1 },
        std::array<int, 3> { 3, 1, 2 },
        std::array<int, 3> { 3, 2, 1 }
    };

    for (const auto &vals : mpsVals) {
        for (int ofs = 0; ofs <= 6; ofs++) {
            T34 m(Suit::M, vals[0] + ofs);
            T34 p(Suit::P, vals[1] + ofs);
            T34 s(Suit::S, vals[2] + ofs);
            int match = std::min(total.ct(m), 3)
                    + std::min(total.ct(p), 3)
                    + std::min(total.ct(s), 3);
            if (match > maxMatch) {
                maxMatch = match;
                maxReqs.reset();
                if (total.ct(m) < 3)
                    maxReqs.set(m.id34());
                if (total.ct(p) < 3)
                    maxReqs.set(p.id34());
                if (total.ct(s) < 3)
                    maxReqs.set(s.id34());
            }
        }
    }

    reqs |= maxReqs;
    return 9 - maxMatch;
}

int Huiyu::tbd(std::bitset<34> &reqs, const TileCount &total)
{
    using namespace tiles34;

    const std::array<T34, 14> tumbler {
        1_p, 2_p, 3_p, 4_p, 5_p,
        8_p, 9_p, 2_s, 4_s, 5_s,
        6_s, 8_s, 9_s, 1_y
    };

    for (T34 t : tumbler)
        reqs.set(t.id34());

    return 14 - total.ct(tumbler);
}

int Huiyu::qdqzqx(std::bitset<34> &reqs, const TileCount &total)
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

    if (big > middle && big > small) {
        for (Suit s : { Suit::M, Suit::P, Suit::S })
            for (int v = 7; v <= 9; v++)
                reqs.set(T34(s, v).id34());
        return 14 - big;
    } else if (middle > big && middle > small) {
        for (Suit s : { Suit::M, Suit::P, Suit::S })
            for (int v = 4; v <= 6; v++)
                reqs.set(T34(s, v).id34());
        return 14 - middle;
    } else {
        if (small >= 9)
            for (Suit s : { Suit::M, Suit::P, Suit::S })
                for (int v = 1; v <= 3; v++)
                    reqs.set(T34(s, v).id34());
        return 14 - small;
    }
}

int Huiyu::gtlt5(std::bitset<34> &reqs, const TileCount &total)
{
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

    if (gt > lt) {
        for (Suit s : { Suit::M, Suit::P, Suit::S })
            for (int v = 6; v <= 9; v++)
                reqs.set(T34(s, v).id34());
        return 14 - gt;
    } else {
        for (Suit s : { Suit::M, Suit::P, Suit::S })
            for (int v = 1; v <= 4; v++)
                reqs.set(T34(s, v).id34());
        return 14 - lt;
    }
}



}


