#include "gen.h"
#include "../util/rand.h"
#include "../util/misc.h"



namespace saki
{



Gen::Gen(const Form &form, const Hand &hand, const T37 &pick)
    : form(form)
    , hand(hand)
    , pick(pick)
{
}

Gen Gen::genForm4FuHan(util::Rand &rand, int fu, int han, int selfWind, int roundWind,
                       const Rule &rule, bool ron)
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

Gen Gen::genForm4Mangan(util::Rand &rand, int han, int selfWind, int roundWind,
                        const Rule &rule, bool ron)
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
            && res.form.manganType() != ManganType::HR
            && !res.form.isPrototypalYakuman()) {
            return res;
        }
    }
}

Gen Gen::genForm4(util::Rand &rand, int triCent, int quadCent, int openCent,
                  int selfWind, int roundWind, const Rule &rule, bool ron)
{
    FormCtx ctx;
    ctx.bless = false;
    ctx.roundWind = roundWind;
    ctx.selfWind = selfWind;
    ctx.extraRound = 0;

    // monkey algorithm, terminates within a few (less than 5) loops
    while (true) {
        Hand h = genFormal4(rand, triCent, quadCent, openCent);

        if (ron) {
            T37 pick = h.drawn();
            genInfo(rand, ctx, pick, h, ron, false);
            h.spinOut();
            Form f(h, pick, ctx, rule);
            if (f.hasYaku())
                return Gen(f, h, pick);
        } else {
            genInfo(rand, ctx, h.drawn(), h, ron, false);
            Form f(h, ctx, rule);
            if (f.hasYaku())
                return Gen(f, h, h.drawn());
        }
    }
}

Gen Gen::genForm4F110(util::Rand &rand, int point, int selfWind, const Rule &rule, bool ron)
{
    // there is no 110-1 tsumo
    assert(ron || (point == 10800 || point == 7200));

    while (true) {
        Gen res = genForm4F110Horse(rand, selfWind, rule, ron);
        if (res.form.gain() == point)
            return res;
    }
}

Gen Gen::genForm4F110Han(util::Rand &rand, int han, int selfWind, const Rule &rule, bool ron)
{
    while (true) {
        Gen res = genForm4F110Horse(rand, selfWind, rule, ron);
        if (res.form.fu() == 110 && res.form.han() == han)
            return res;
    }
}

Gen Gen::genForm4F110Horse(util::Rand &rand, int selfWind, const Rule &rule, bool ron)
{
    FormCtx ctx;
    ctx.bless = false;
    ctx.roundWind = selfWind;
    ctx.selfWind = selfWind;
    ctx.extraRound = 0;

    while (true) {
        TileCount closed;
        util::Stactor<M37, 4> barks;

        // must be this pair
        closed.inc(T37(Suit::F, selfWind), 2);

        // extactly two ankans
        T37 t(tiles34::YAO13[rand.gen(13)].id34());
        barks.pushBack(M37::ankan(t, t, t, t));
        t = T37(tiles34::YAO13[rand.gen(13)].id34());
        barks.pushBack(M37::ankan(t, t, t, t));

        // monkeyly gen the rest things
        for (int i = 0; i < 2; i++) {
            bool open = rand.gen(100) < 20; // 20% open

            if (rand.gen(100) < 50) { // 50% tri
                T37 t(rand.gen(34));

                if (rand.gen(100) < 5) { // gen quad (must open)
                    barks.pushBack(M37::daiminkan(t, t, t, t, 0));
                } else { // gen tri
                    if (open)
                        barks.pushBack(M37::pon(t, t, t, 0));
                    else
                        closed.inc(t, 3);
                }
            } else { // gen seq
                Suit s = static_cast<Suit>(rand.gen(3));
                T37 l(s, rand.gen(7) + 1); // range 1 ~ 7
                T37 m(l.next().id34());
                T37 r(m.next().id34());
                if (open) {
                    barks.pushBack(M37::chii(l, m, r, 1));
                } else {
                    closed.inc(l, 1);
                    closed.inc(m, 1);
                    closed.inc(r, 1);
                }
            }
        }

        // drop one as pick, store as drawn
        util::Stactor<T37, 13> ts = closed.t37s13();
        int drop = rand.gen(ts.size());
        closed.inc(ts[drop], -1);
        Hand h(closed, barks);
        h.draw(ts[drop]);

        if (!h.over4()) {
            if (ron) {
                T37 pick = h.drawn();
                genInfo(rand, ctx, pick, h, ron, false);
                h.spinOut();
                Form f(h, pick, ctx, rule);
                if (f.hasYaku())
                    return Gen(f, h, pick);
            } else {
                genInfo(rand, ctx, h.drawn(), h, ron, false);
                Form f(h, ctx, rule);
                if (f.hasYaku())
                    return Gen(f, h, h.drawn());
            }
        }
    }
}

Hand Gen::genFormal4(util::Rand &rand, int triCent, int quadCent, int openCent)
{
    // monkey algorithm, usually ends within a few (less than 3) loops
    while (true) {
        Hand h = genWild4(rand, triCent, quadCent, openCent);
        if (!h.over4())
            return h;
    }
}

Hand Gen::genWild4(util::Rand &rand, int triCent, int quadCent, int openCent)
{
    TileCount closed;
    util::Stactor<M37, 4> barks;

    for (int i = 0; i < 4; i++) {
        bool open = rand.gen(100) < openCent;

        if (rand.gen(100) < triCent) {
            T37 t(rand.gen(34));

            if (rand.gen(100) < quadCent) { // gen quad
                if (open)
                    barks.pushBack(M37::daiminkan(t, t, t, t, 0));
                else
                    barks.pushBack(M37::ankan(t, t, t, t));
            } else { // gen tri
                if (open)
                    barks.pushBack(M37::pon(t, t, t, 1));
                else
                    closed.inc(t, 3);
            }
        } else {
            Suit s = static_cast<Suit>(rand.gen(3));
            T37 l(s, rand.gen(7) + 1); // range 1 ~ 7
            T37 m(l.next().id34());
            T37 r(m.next().id34());
            if (open) {
                barks.pushBack(M37::chii(l, m, r, 1));
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
    auto ts = closed.t37s13();
    int drop = rand.gen(ts.size());
    closed.inc(ts[drop], -1);
    Hand res(closed, barks);
    res.draw(ts[drop]);

    return res;
}

void Gen::genInfo(util::Rand &rand, FormCtx &ctx, T34 pick, const Hand &h, bool ron, bool f110)
{
    const int RIICHI_CENT = 40;
    const int DABURU_CENT = f110 ? 50 : 10;
    const int IPPATSU_CENT = f110 ? 30 : 10;
    const int RINSHAN_CENT = 20;
    const int HAITEI_CENT = f110 ? 20 : 3;
    const int HOUTEI_CENT = f110 ? 20 : 3;
    const int CHANKAN_CENT = f110 ? 20 : 1;

    ctx.riichi = 0;
    ctx.ippatsu = false;
    if (h.isMenzen() && rand.gen(100) < RIICHI_CENT) {
        ctx.riichi = 1;
        if (rand.gen(100) < DABURU_CENT)
            ctx.riichi = 2;

        if (rand.gen(100) < IPPATSU_CENT)
            ctx.ippatsu = true;
    }

    if (!ctx.ippatsu) {
        bool hasKan = util::any(h.barks(), [](const M37 &m) { return m.isKan(); });

        // *INDENT-OFF*
        if (!ron && hasKan && rand.gen(100) < RINSHAN_CENT)
            ctx.duringKan = true;
        else if (!ron && ctx.riichi != 2 && rand.gen(100) < HAITEI_CENT)
            ctx.emptyMount = true;
        else if (ron && rand.gen(100) < HOUTEI_CENT)
            ctx.emptyMount = true;
        else if (ron && h.ct(pick) == 1 && rand.gen(100) < CHANKAN_CENT)
            ctx.duringKan = true; // '== 1' since haven't spin-out
        // *INDENT-ON*
    }
}



} // namespace saki
