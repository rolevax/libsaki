#include "gen.h"
#include "rand.h"
#include "util.h"

#include <cassert>



namespace saki
{



Gen::Gen(const Form &form, const Hand &hand, const T37 &pick)
    : form(form)
    , hand(hand)
    , pick(pick)
{
}

/*
Gen Gen::genForm4Point(int point, int selfWind, int roundWind,
                       const RuleInfo &rule, bool ron)
{
    static std::array<int, 19> valDT = {
        1500, 2100, 2400, 3000, 3600, 3900, 4500, 4800, 6900, 7800,
        8700, 9600, 10800, 11700, 12000, 18000, 24000, 36000, 48000
    };

    static std::array<int, 21> valDR = {
        1500, 2000, 2400, 2900, 3400, 3900, 4400, 4800, 5300, 5800,
        6800, 7700, 8700, 9600, 10600, 11600, 12000, 18000, 24000, 36000,
        48000
    };

    static std::array<int, 20> valNT = {
        1100, 1500, 1600, 2000, 2400, 2700, 3100, 3200, 4000, 4700,
        5200, 5900, 6400, 7200, 7900, 8000, 12000, 16000, 24000, 32000
    };

    static std::array<int, 21> valNR = {
        1000, 1300, 1600, 2000, 2300, 2600, 2900, 3200, 3600, 3900,
        4500, 5200, 5800, 6400, 7100, 7700, 8000, 12000, 16000, 24000,
        32000
    };

    assert(1 <= selfWind && selfWind <= 4);
    assert(1 <= roundWind && roundWind <= 4);
    if (selfWind == 1) { // dealer
        if (ron)
            assert(std::find(valDR.begin(), valDR.end(), point) != valDR.end());
        else
            assert(std::find(valDT.begin(), valDT.end(), point) != valDT.end());
    } else { // non-dealer
        if (ron)
            assert(std::find(valNR.begin(), valNR.end(), point) != valNR.end());
        else
            assert(std::find(valNT.begin(), valNT.end(), point) != valNT.end());
    }

    //
    bool f90 = (selfWind == 1
            && (point == 4500 || point == 4400 || point == 8700))
            || (selfWind != 1
            && (point == 3100 || point == 5900 || point == 2900 || point == 5800));
    bool f110 = (selfWind == 1
                 && (point == 5300 || point == 10600 || point == 10800))
            || (selfWind != 1
            && (point == 3600 || point == 7100 || point == 7200));

    int tri, quad, open;
    tri = f90 ? 60 : 20;
    quad = f90 ? 70 : 10;
    open = f90 ? 2 : 30;

    if (f110) {
        assert(selfWind == roundWind);
        return genForm4F110(point, selfWind, rule, ron);
    } else { // common cases
        while (true) {
            Gen res = genForm4(tri, quad, open, selfWind, roundWind, rule, ron);
            if (res.form->getGain() == point)
                return res;
        }
    }
}
*/

Gen Gen::genForm4FuHan(Rand &rand, int fu, int han, int selfWind, int roundWind,
                       const RuleInfo &rule, bool ron)
{
    assert(1 <= selfWind && selfWind <= 4);
    assert(1 <= roundWind && roundWind <= 4);

    int tri, quad, open;
    tri = fu >= 80 ? 60 : 20;
    quad = fu >= 80 ? 70 : 10;
    open = fu >= 80 ? 2 : 30;

    if (fu == 110) {
        assert(selfWind == roundWind);
        return genForm4F110Han(rand, han, selfWind, rule, ron);
    } else { // common cases
        while (true) {
            Gen res = genForm4(rand, tri, quad, open, selfWind, roundWind, rule, ron);
            if (res.form.fu() == fu && res.form.han() == han)
                return res;
        }
    }
}

Gen Gen::genForm4Mangan(Rand &rand, int han, int selfWind, int roundWind,
                        const RuleInfo &rule, bool ron)
{
    assert(1 <= selfWind && selfWind <= 4);
    assert(1 <= roundWind && roundWind <= 4);

    int tri, quad, open;
    tri = 20;
    quad = 10;
    open = 5;

    while (true) {
        Gen res = genForm4(rand, tri, quad, open, selfWind, roundWind, rule, ron);
        if (res.form.han() == han
                && res.form.gain() >= 8000
                && !res.form.isPrototypalYakuman()) {
            return res;
        }
    }
}

Gen Gen::genForm4(Rand &rand, int triCent, int quadCent, int openCent,
                  int selfWind, int roundWind, const RuleInfo &rule, bool ron)
{
    PointInfo info;
    info.bless = false;
    info.roundWind = roundWind;
    info.selfWind = selfWind;
    info.extraRound = 0;

    // monkey algorithm, terminates within a few (less than 5) loops
    while (true) {
        Hand h = genFormal4(rand, triCent, quadCent, openCent);

        if (ron) {
            T37 pick = h.drawn();
            genInfo(rand, info, pick, h, ron, false);
            h.spinOut();
            Form f(h, pick, info, rule);
            if (f.hasYaku())
                return Gen(f, h, pick);
        } else {
            genInfo(rand, info, h.drawn(), h, ron, false);
            Form f(h, info, rule);
            if (f.hasYaku())
                return Gen(f, h, h.drawn());
        }
    }
}

Gen Gen::genForm4F110(Rand &rand, int point, int selfWind, const RuleInfo &rule, bool ron)
{
    // there is no 110-1 tsumo
    assert(ron || (point == 10800 || point == 7200));

    while (true) {
        Gen res = genForm4F110Horse(rand, selfWind, rule, ron);
        if (res.form.gain() == point)
            return res;
    }
}

Gen Gen::genForm4F110Han(Rand &rand, int han, int selfWind, const RuleInfo &rule, bool ron)
{
    while (true) {
        Gen res = genForm4F110Horse(rand, selfWind, rule, ron);
        if (res.form.fu() == 110 && res.form.han() == han)
            return res;
    }
}

Gen Gen::genForm4F110Horse(Rand &rand, int selfWind, const RuleInfo &rule, bool ron)
{
    PointInfo info;
    info.bless = false;
    info.roundWind = selfWind;
    info.selfWind = selfWind;
    info.extraRound = 0;

    while (true) {
        TileCount closed;
        std::vector<M37> barks;

        // must be this pair
        closed.inc(T37(Suit::F, selfWind), 2);

        // extactly two ankans
        T37 t(tiles34::YAO13[rand.gen(13)].id34());
        barks.push_back(M37::ankan(t, t, t, t));
        t = T37(tiles34::YAO13[rand.gen(13)].id34());
        barks.push_back(M37::ankan(t, t, t, t));

        // monkeyly gen the rest things
        for (int i = 0; i < 2; i++) {
            bool open = rand.gen(100) < 20; // 20% open

            if (rand.gen(100) < 50) { // 50% tri
                T37 t(rand.gen(34));

                if (rand.gen(100) < 5) { // gen quad (must open)
                    barks.push_back(M37::daiminkan(t, t, t, t, 0));
                } else { // gen tri
                    if (open)
                        barks.push_back(M37::pon(t, t, t, 0));
                    else
                        closed.inc(t, 3);
                }
            } else { // gen seq
                Suit s = static_cast<Suit>(rand.gen(3));
                T37 l(s, rand.gen(7) + 1); // range 1 ~ 7
                T37 m(l.next().id34());
                T37 r(m.next().id34());
                if (open) {
                    barks.push_back(M37::chii(l, m, r, 1));
                } else {
                    closed.inc(l, 1);
                    closed.inc(m, 1);
                    closed.inc(r, 1);
                }
            }
        }

        // drop one as pick, store as drawn
        std::vector<T37> ts = closed.t37s();
        int drop = rand.gen(ts.size());
        closed.inc(ts[drop], -1);
        Hand h(closed, barks);
        h.draw(ts[drop]);

        if (!h.over4()) {
            if (ron) {
                T37 pick = h.drawn();
                genInfo(rand, info, pick, h, ron, false);
                h.spinOut();
                Form f(h, pick, info, rule);
                if (f.hasYaku())
                    return Gen(f, h, pick);
            } else {
                genInfo(rand, info, h.drawn(), h, ron, false);
                Form f(h, info, rule);
                if (f.hasYaku())
                    return Gen(f, h, h.drawn());
            }
        }
    }
}

Hand Gen::genFormal4(Rand &rand, int triCent, int quadCent, int openCent)
{
    // monkey algorithm, usually ends within a few (less than 3) loops
    while (true) {
        Hand h = genWild4(rand, triCent, quadCent, openCent);
        if (!h.over4())
            return h;
    }
}

Hand Gen::genWild4(Rand &rand, int triCent, int quadCent, int openCent)
{
    TileCount closed;
    std::vector<M37> barks;

    for (int i = 0; i < 4; i++) {
        bool open = rand.gen(100) < openCent;

        if (rand.gen(100) < triCent) {
            T37 t(rand.gen(34));

            if (rand.gen(100) < quadCent) { // gen quad
                if (open)
                    barks.push_back(M37::daiminkan(t, t, t, t, 0));
                else
                    barks.push_back(M37::ankan(t, t, t, t));
            } else { // gen tri
                if (open)
                    barks.push_back(M37::pon(t, t, t, 1));
                else
                    closed.inc(t, 3);
            }
        } else {
            Suit s = static_cast<Suit>(rand.gen(3));
            T37 l(s, rand.gen(7) + 1); // range 1 ~ 7
            T37 m(l.next().id34());
            T37 r(m.next().id34());
            if (open) {
                barks.push_back(M37::chii(l, m, r, 1));
            } else {
                closed.inc(l, 1);
                closed.inc(m, 1);
                closed.inc(r, 1);
            }
        }
    }

    // make pair
    T37 t(rand.gen(34));
    closed.inc(t, 2);

    // drop one as pick, store as drawn
    std::vector<T37> ts = closed.t37s();
    int drop = rand.gen(ts.size());
    closed.inc(ts[drop], -1);
    Hand res(closed, barks);
    res.draw(ts[drop]);

    return res;
}

void Gen::genInfo(Rand &rand, PointInfo &info, T34 pick, const Hand &h, bool ron, bool f110)
{
    const int RIICHI_CENT = 40;
    const int DABURU_CENT = f110 ? 50 : 10;
    const int IPPATSU_CENT = f110 ? 30 : 10;
    const int RINSHAN_CENT = 20;
    const int HAITEI_CENT = f110 ? 20 : 3;
    const int HOUTEI_CENT = f110 ? 20 : 3;
    const int CHANKAN_CENT = f110 ? 20 : 1;

    info.riichi = 0;
    info.ippatsu = false;
    if (h.isMenzen() && rand.gen(100) < RIICHI_CENT) {
        info.riichi = 1;
        if (rand.gen(100) < DABURU_CENT)
            info.riichi = 2;
        if (rand.gen(100) < IPPATSU_CENT)
            info.ippatsu = true;
    }

    if (!info.ippatsu) {
        bool hasKan = util::any(h.barks(), [](const M37 &m) { return m.isKan(); });

        if (!ron && hasKan && rand.gen(100) < RINSHAN_CENT)
            info.duringKan = true;
        else if (!ron && info.riichi != 2 && rand.gen(100) < HAITEI_CENT)
            info.emptyMount = true;
        else if (ron && rand.gen(100) < HOUTEI_CENT)
            info.emptyMount = true;
        else if (ron && h.ct(pick) == 1 && rand.gen(100) < CHANKAN_CENT)
            info.duringKan = true; // '== 1' since haven't spin-out
    }
}



} // namespace saki


