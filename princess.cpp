#include "princess.h"
#include "rand.h"
#include "util.h"
#include "debug_cheat.h"



namespace saki
{



Princess::Princess(const Table &table, Rand &rand, Mount &mount,
                   const std::array<std::unique_ptr<Girl>, 4> &girls)
    : mTable(table)
    , mRand(rand)
    , mMount(mount)
    , mGirls(girls)
{
    mHasImageIndics.fill(false);
}

std::array<Hand, 4> Princess::deal()
{
    std::array<TileCount, 4> inits = nonMonkey();
    return monkey(inits);
}

bool Princess::imagedAsDora(T34 t, Princess::Indic which) const
{
    int i = static_cast<int>(which);
    return mHasImageIndics[i] && mImageIndics[i] % t;
}

bool Princess::mayHaveDora(T34 t) const
{
    return imagedAsDora(t, Indic::DORA)
            || imagedAsDora(t, Indic::URADORA)
            || imagedAsDora(t, Indic::KANDORA)
            || imagedAsDora(t, Indic::KANURA);
}

bool Princess::hasImageIndic(Indic which) const
{
    return mHasImageIndics[static_cast<int>(which)];
}

T34 Princess::getImageIndic(Indic which) const
{
    assert(hasImageIndic(which));
    return mImageIndics[static_cast<int>(which)];
}

const Table &Princess::getTable() const
{
    return mTable;
}

std::array<TileCount, 4> Princess::nonMonkey()
{
    std::array<TileCount, 4> res;

#ifdef LIBSAKI_CHEAT_PRINCESS
    for (size_t pos = 0; pos < cheat::wall.size(); pos++)
        mMount.pin(Mount::WALL, pos, cheat::wall[pos]);

    for (size_t pos = 0; pos < cheat::dead.size(); pos++)
        mMount.pin(Mount::DEAD, pos, cheat::dead[pos]);

    for (int w = 0; w < 4; w++)
        for (const T37 &t : cheat::inits[w])
            res[w].inc(mMount.initPopExact(t), 1);
#else
    std::bitset<Girl::NUM_NM_SKILL> presence;

    using Id = Girl::Id;
    static const std::vector<Id> CHECK_ORDER1 {
        Id::SHIBUYA_TAKAMI,
        Id::USUZUMI_HATSUMI,
        Id::SHISHIHARA_SAWAYA,
        Id::IWATO_KASUMI
    };

    for (Id id : CHECK_ORDER1) {
        Who who = mTable.findGirl(id);
        if (who.somebody())
            mGirls[who.index()]->nonMonkey(mRand, res[who.index()], mMount, presence, *this);
    }

    doraMatters();

    static const std::vector<Id> CHECK_ORDER2 {
        Id::OOHOSHI_AWAI
    };

    for (Id id : CHECK_ORDER2) {
        Who who = mTable.findGirl(id);
        if (who.somebody())
            mGirls[who.index()]->nonMonkey(mRand, res[who.index()], mMount, presence, *this);
    }
#endif

    return res;
}

std::array<Hand, 4> Princess::monkey(std::array<TileCount, 4> &inits)
{
    std::array<Hand, 4> res;
    std::array<Exist, 4> exists;

    for (auto &g : mGirls)
        g->onMonkey(exists, *this);

    // there should not be any check condition in checkInit()
    // that is relied on enough remaining certain tiles in mount
    // because one is checked after another girl's init hand is
    // fully confirmed
    for (int w = 0; w < 4; w++) {
        for (int iter = 0; true; iter++) {
            Mount mount(mMount);
            TileCount init(inits[w]); // copy
            Exist exist(exists[w]); // copy

            mount.initFill(mRand, init, exist);
            Hand hand(init);

            auto pass = [w, &hand, this, &mount, iter](int checker) {
                return mGirls[checker]->checkInit(Who(w), hand, *this, iter);
            };

            if (pass(0) && pass(1) && pass(2) && pass(3)) {
                mMount = mount;
                res[w] = hand;
                break;
            }
        }
    }

    return res;
}

void Princess::doraMatters()
{
    using namespace tiles37;

    // NONE: the indicator will be scientifically choosen
    // ANY:  the indicator will be choosen from B-space by a one-hot distro
    // WALL: same as ANY, and the indicated dora must remain 4 in A-space
    enum class Fix { NONE, ANY, WALL };

    std::array<Fix, 4> fixes;
    fixes.fill(Fix::NONE);

    auto update = [&fixes](Indic which, Fix rhs) {
        Fix &lhs = fixes[int(which)];
        // overwrite by priority
        lhs = std::max(lhs, rhs);
    };

    // exluded by numeric value, regardless of red/black
    std::array<std::array<bool, 34>, 4> ex34ss;
    for (auto &ex34s : ex34ss)
        ex34s.fill(false);

    Who kuro = mTable.findGirl(Girl::Id::MATSUMI_KURO);
    Who awai = mTable.findGirl(Girl::Id::OOHOSHI_AWAI);

    if (awai.somebody()) {
        update(Indic::DORA, Fix::ANY);
        update(Indic::URADORA, Fix::ANY);
        update(Indic::KANDORA, Fix::ANY);
        update(Indic::KANURA, Fix::WALL);

        // prevent yakuhai to ensure Awai::checkInit() always return
        auto &ex34s = ex34ss[static_cast<int>(Indic::KANURA)];
        ex34s[(1_y).id34()] = true;
        ex34s[(2_y).id34()] = true;
        ex34s[(3_y).id34()] = true;
        ex34s[T34(Suit::F, mTable.getSelfWind(awai)).indicator().id34()] = true;
        ex34s[T34(Suit::F, mTable.getRoundWind()).indicator().id34()] = true;

        // exclude akadoras to limit awai's point
        if (mTable.getRuleInfo().akadora != TileCount::AKADORA0) {
            ex34s[(4_m).id34()] = true;
            ex34s[(4_p).id34()] = true;
            ex34s[(4_s).id34()] = true;
        }
    }

    if (kuro.somebody()) {
        update(Indic::DORA, Fix::WALL);

        // prevent akadora from being pilfered by awai or someone
        // by exluding number-4 to be an indicator
        if (mTable.getRuleInfo().akadora != TileCount::AKADORA0) {
            // start from '1' since the normal dora lives or dies
            // together with akadoras
            for (int i = 1; i < 4; i++) {
                ex34ss[i][(4_m).id34()] = true;
                ex34ss[i][(4_p).id34()] = true;
                ex34ss[i][(4_s).id34()] = true;
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (fixes[i] != Fix::NONE) {
            fixIndicator(Indic(i), ex34ss[i], fixes[i] == Fix::WALL);

            // make fixed doras always unique for redundant safety
            // (needed by at least awai)
            // use id34() instread of id37() to ensure uniqueness
            // note that a fixed indicator might equal to a non-fixed
            // scientific indicator. don't know whether this will cause bug...
            for (int j = i; j < 4; j++)
                ex34ss[j][mImageIndics[i].id34()] = true;
        }
    }
}

void Princess::fixIndicator(Indic which, const std::array<bool, 34> &exceptId34s, bool wall)
{
    int i = static_cast<int>(which);
    mImageIndics[i] = pickIndicator(exceptId34s, wall);
    Mount::Exit exit = i % 2 == 0 ? Mount::DORA : Mount::URADORA;
    int pos = i / 2 == 0 ? 0 : 1;

    T37 indic(mImageIndics[i].id34());
    if (mMount.remainA(indic) == 0)
        indic = indic.toAka5();
    mMount.loadB(indic, 1);
    mImageIndics[static_cast<int>(which)] = indic;
    mHasImageIndics[static_cast<int>(which)] = true;

    for (int ti = 0; ti < 34; ti++) {
        T34 t(ti);
        // TODO de-magic the '100's and parametize them
        mMount.power(exit, pos, t, -100, false);
        mMount.power(exit, pos, t, t == indic ? 100 : -100, true);
    }
}

T34 Princess::pickIndicator(const std::array<bool, 34> &ex34s, bool wall)
{
    // be careful with the 34/37 problem
    // equality is checked by id34, and indicator is choosen in 37-domain

    std::vector<T34> indicatable;

    for (int ti = 0; ti < 34; ti++) {
        T34 t(ti);
        if (!ex34s[ti]
                && mMount.remainA(t) > 0
                && (!wall || mMount.remainA(t.dora()) == 4)) {
            indicatable.push_back(t);
        }
    }

    assert(!indicatable.empty());
    return indicatable[mRand.gen(indicatable.size())];
}



} // namespace saki


