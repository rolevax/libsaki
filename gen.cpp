#include "gen.h"
#include "rand.h"

#include <algorithm>
#include <iostream>
#include <array>
#include <cassert>


/*
Gen::Gen(std::unique_ptr<const Form> &form, const Hand &hand, Tile pick)
    : form(std::move(form))
    , hand(hand)
    , pick(pick)
{

}

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

    // TODO
    // 2. receive more param
    // - dora vector (Hand::count(tile))
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

Gen Gen::genForm4FuHan(int fu, int han, int selfWind, int roundWind,
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
        return genForm4F110Han(han, selfWind, rule, ron);
    } else { // common cases
        while (true) {
            Gen res = genForm4(tri, quad, open, selfWind, roundWind, rule, ron);
            if (res.form->as4().getFu() == fu && res.form->as4().getHan() == han)
                return res;
        }
    }
}

Gen Gen::genForm4Mangan(int han, int selfWind, int roundWind,
                        const RuleInfo &rule, bool ron)
{
    assert(1 <= selfWind && selfWind <= 4);
    assert(1 <= roundWind && roundWind <= 4);

    int tri, quad, open;
    tri = 20;
    quad = 10;
    open = 5;

    while (true) {
        Gen res = genForm4(tri, quad, open, selfWind, roundWind, rule, ron);
        if (res.form->as4().getHan() == han
                && res.form->getGain() >= 8000
                && !res.form->isClassicalYakuman())
            return res;
    }
}

Gen Gen::genForm4(int triCent, int quadCent, int openCent,
                  int selfWind, int roundWind, const RuleInfo &rule, bool ron)
{
    PointInfo info;
    info.bless = false;
    info.roundWind = roundWind;
    info.selfWind = selfWind;
    info.extraRound = 0;
    info.doraCt = 0;
    info.uraCt = 0;
    info.akaCt = 0;

    // monkey algorithm, terminates within a few (less than 5) loops
    while (true) {
        Hand h = genFormal4(triCent, quadCent, openCent);

        int pi = myRand() % h.getHand().size();
        Tile pick = h.getHand().at(pi);

        genInfo(info, pick, h, ron, false);
        h.getHand().erase(h.getHand().begin() + pi);
        std::unique_ptr<const Form> f = Form::make(h, pick, ron, info, rule);
        if (f->hasYaku())
            return Gen(f, h, pick);
    }
}

Gen Gen::genForm4F110(int point, int selfWind, const RuleInfo &rule, bool ron)
{
    // there is no 110-1 tsumo
    assert(ron || (point == 10800 || point == 7200));
    while (true) {
        Gen res = genForm4F110Horse(selfWind, rule, ron);
        if (res.form->getGain() == point)
            return res;
    }
}

Gen Gen::genForm4F110Han(int han, int selfWind, const RuleInfo &rule, bool ron)
{
    while (true) {
        Gen res = genForm4F110Horse(selfWind, rule, ron);
        if (res.form->as4().getFu() == 110 && res.form->as4().getHan() == han)
            return res;
    }
}

Gen Gen::genForm4F110Horse(int selfWind, const RuleInfo &rule, bool ron)
{
    PointInfo info;
    info.bless = false;
    info.roundWind = selfWind;
    info.selfWind = selfWind;
    info.extraRound = 0;
    info.doraCt = 0;
    info.uraCt = 0;
    info.akaCt = 0;

    while (true) {
        std::vector<Tile> hand;
        std::vector<Meld> barks;

        // must be this pair
        hand.emplace_back(Suit::F, selfWind);
        hand.emplace_back(Suit::F, selfWind);

        // extactly two ankans
        int i = myRand() % 13;
        barks.emplace_back(Tiles::YAO13[i], Tiles::YAO13[i],
                           Tiles::YAO13[i], Tiles::YAO13[i], Meld::NO_OPEN);
        i = myRand() % 13;
        barks.emplace_back(Tiles::YAO13[i], Tiles::YAO13[i],
                           Tiles::YAO13[i], Tiles::YAO13[i], Meld::NO_OPEN);

        // monkeyly gen the rest things
        for (int i = 0; i < 2; i++) {
            bool open = myRand() % 100 < 20; // 20% open

            if (myRand() % 100 < 50) { // 50% tri
                Tile t(myRand() % 34);

                if (myRand() % 100 < 5) { // gen quad (must open)
                    barks.emplace_back(t, t, t, t, 0);
                } else { // gen tri
                    if (open) {
                        barks.emplace_back(t, t, t, 1);
                    } else {
                        hand.push_back(t);
                        hand.push_back(t);
                        hand.push_back(t);
                    }
                }
            } else { // gen seq
                int suitId = myRand() % 3;
                Suit s = suitId == 0 ? Suit::M
                                     : suitId == 1 ? Suit::P : Suit::S;
                int l = myRand() % 7 + 1; // range 1 ~ 7
                int m = l + 1;
                int r = m + 1;
                if (open) {
                    barks.emplace_back(Tile(s, l), Tile(s, m), Tile(s, r), 1);
                } else {
                    hand.emplace_back(s, l);
                    hand.emplace_back(s, m);
                    hand.emplace_back(s, r);
                }
            }
        }

        std::sort(hand.begin(), hand.end());
        Hand h(hand, barks);

        if (!h.verticalOverflow()) {
            int pi = myRand() % h.getHand().size();
            Tile pick = h.getHand().at(pi);

            genInfo(info, pick, h, ron, true);

            h.getHand().erase(h.getHand().begin() + pi);
            std::unique_ptr<const Form> f = Form::make(h, pick, ron, info, rule);
            if (f->hasYaku())
                return Gen(f, h, pick);
        }
    }
}

Hand Gen::genFormal4(int triCent, int quadCent, int openCent)
{
    // monkey algorithm, terminates within a few (less than 3) loops
    while (true) {
        Hand h = genWild4(triCent, quadCent, openCent);
        if (!h.verticalOverflow())
            return h;
    }
}

Hand Gen::genWild4(int triCent, int quadCent, int openCent)
{
    std::vector<Tile> hand;
    std::vector<Meld> barks;

    for (int i = 0; i < 4; i++) {
        bool open = myRand() % 100 < openCent;

        if (myRand() % 100 < triCent) {
            Tile t(myRand() % 34);

            if (myRand() % 100 < quadCent) { // gen quad
                if (open) {
                    barks.emplace_back(t, t, t, t, 0);
                } else {
                    barks.emplace_back(t, t, t, t, Meld::NO_OPEN);
                }
            } else { // gen tri
                if (open) {
                    barks.emplace_back(t, t, t, 1);
                } else {
                    hand.push_back(t);
                    hand.push_back(t);
                    hand.push_back(t);
                }
            }
        } else {
            int suitId = myRand() % 3;
            Suit s = suitId == 0 ? Suit::M
                                 : suitId == 1 ? Suit::P : Suit::S;
            int l = myRand() % 7 + 1; // range 1 ~ 7
            int m = l + 1;
            int r = m + 1;
            if (open) {
                barks.emplace_back(Tile(s, l), Tile(s, m), Tile(s, r), 1);
            } else {
                hand.emplace_back(s, l);
                hand.emplace_back(s, m);
                hand.emplace_back(s, r);
            }
        }
    }

    // make pair
    Tile t(myRand() % 34);
    hand.push_back(t);
    hand.push_back(t);

    std::sort(hand.begin(), hand.end());

    return Hand(hand, barks);
}

void Gen::genInfo(PointInfo &info, Tile &pick, const Hand &h, bool ron, bool f110)
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
    if (h.isMenzen() && myRand() % 100 < RIICHI_CENT) {
        info.riichi = 1;
        if (myRand() % 100 < DABURU_CENT)
            info.riichi = 2;
        if (myRand() % 100 < IPPATSU_CENT)
            info.ippatsu = true;
    }

    if (!info.ippatsu) {
        bool hasKan = false;
        for (size_t i = 0; i < h.getBarks().size(); i++) {
            const Meld &m = h.getBarks().at(i);
            if (m.isQuad()) {
                if (info.riichi) { // check kannability for kan-after-riichi
                // FUCK this part originally commented-out, forgot why
//                    assert(m.isAnkan());
//                    // revert the rinshan-kaihou process
//                    Hand tmp(h);
//                    tmp.getBarks().erase(tmp.getBarks().begin() + i);
//                    tmp.getHand().erase(tmp.getHand().begin() + pi);
//                    Hand::insert(tmp.getHand(), m[0]);
//                    Hand::insert(tmp.getHand(), m[1]);
//                    Hand::insert(tmp.getHand(), m[2]);
//                    if (Form::notInSeq(tmp, m[3])) {
//                        hasKan = true;
//                        break;
//                    }
                } else {
                    hasKan = true;
                    break;
                }
            }
        }

        if (!ron && hasKan && myRand() % 100 < RINSHAN_CENT)
            info.duringKan = true;
        else if (!ron && info.riichi != 2 && myRand() % 100 < HAITEI_CENT)
            info.emptyMount = true;
        else if (ron && myRand() % 100 < HOUTEI_CENT)
            info.emptyMount = true;
        else if (ron && h.count(pick) == 1 && myRand() % 100 < CHANKAN_CENT)
            info.duringKan = true; // count == 1 since haven't erased
    }
}
                    */



