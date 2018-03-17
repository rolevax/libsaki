#include "shiraitodai_awai.h"
#include "../table/princess.h"
#include "../table/table.h"
#include "../table/table_view_hand.h"
#include "../ai/ai.h"
#include "../util/misc.h"

#include <sstream>



namespace saki
{



bool Awai::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    (void) table;

    if (who == mSelf || iter > 500)
        return true;

    return init.step() >= (iter > 300 ? 4 : 5);
}

void Awai::onMonkey(std::array<Exist, 4> &exists, const Table &table)
{
    if (!usingDaburii())
        return;

    int self = mSelf.index();

    T34 dora = table.getMount().getDrids().front().dora();
    exists[self].inc(dora, -EJECT_MK);

    using namespace tiles37;
    exists[self].inc(0_m, -EJECT_MK);
    exists[self].inc(0_p, -EJECT_MK);
    exists[self].inc(0_s, -EJECT_MK);
}

void Awai::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf
        && who == table.getDealer()
        && table.getRiver(who).empty()
        && table.getHand(who).barks().empty()) {
        // dealer's very first draw, slow her down
        for (T34 t : table.getHand(who).effA())
            mount.lightA(t, -ACCEL_MK);

        return;
    }

    if (rinshan || !usingDaburii())
        return;

    if (who == mSelf && table.getRiver(who).empty()) {
        // self's first turn. draw the necessary tile
        // if already ready, reserve probablity of heavenly hand
        if (mNeedFirstDraw) {
            mNeedFirstDraw = false;
            for (const T37 &t : tiles37::ORDER37) {
                mount.lightA(t, -EJECT_MK);
                mount.lightB(t, t == mFirstDraw && !t.isAka5() ? DRAG_MK : -EJECT_MK);
            }
        }
    } else {
        const auto &barks = table.getHand(mSelf).barks();
        int barkCt = barks.size();
        if (barkCt > 0
            && util::any(barks, [](const M37 &m) { return m.type() == M37::Type::ANKAN; })
            && mount.remainPii() <= lastCorner(table.getDice(), 4 - mount.remainRinshan())) {
            // after kan and corner, for every player,
            // make wait-tile appear in mount continuously
            for (T34 t : tiles34::ALL34) {
                mount.lightA(t, -EJECT_MK);
                mount.lightB(t, util::has(mLastWaits, t) ? ACCEL_MK : -EJECT_MK);
            }
        } else if (who == mSelf) {
            // before kan
            int tail = lastCorner(table.getDice(), 4 - mount.remainRinshan());
            int rem = mount.remainPii();
            if (tail < rem && rem <= tail + 4) {
                // before last corner, extract kan-material
                for (T34 t : tiles34::ALL34) {
                    mount.lightA(t, -EJECT_MK);
                    mount.lightB(t, t == mKanura ? DRAG_MK : -EJECT_MK);
                }
            } else {
                // don't tsumo too early
                accelerate(mount, table.getHand(mSelf), River(), -EJECT_MK);
                // don't get the kan-material which is not kanura
                // (awai can have 2 closed triplets in init hand)
                for (T34 t : tiles34::ALL34)
                    if (table.getHand(mSelf).closed().ct(t) == 3)
                        mount.lightA(t, -EJECT_MK);
            }
        }
    }
}

HrhBargainer *Awai::onHrhBargain(const Table &table)
{
    if (!usingDaburii())
        return nullptr;

    int rw = table.getRoundWind();
    int sw = table.getSelfWind(mSelf);

    for (int v = 1; v <= 4; v++)
        if (v != rw && v != sw)
            mSomeGuestWind = T34(Suit::F, v);

    return this;
}

HrhBargainer::Claim Awai::hrhBargainClaim(int plan, T34 t)
{
    T34 kanura = kanuraOfPlan(plan);
    return t == kanura ? Claim::FOUR : (t % kanura ? Claim::ANY : Claim::NONE);
}

int Awai::hrhBargainPlanCt()
{
    return 25; // 3x8 no-fives + 1 some guest wind
}

void Awai::onHrhBargained(int plan, Mount &mount)
{
    mKanura = kanuraOfPlan(plan);

    // reserve kan-material
    mount.loadB(T37(mKanura.id34()), 1);

    // fix kanura indicator
    mount.loadB(T37(mKanura.indicator().id34()), 1);
    for (T34 t : tiles34::ALL34) {
        mount.incMk(Mount::URAHYOU, 1, t, -100, false);
        mount.incMk(Mount::URAHYOU, 1, t, t % mKanura ? 100 : -100, true);
    }

    // make other indics diff from the kanura indic
    // useful only in a kuro-less case
    // if kuro is present, the conflict is already solved by bargain
    mount.incMk(Mount::DORAHYOU, 0, mKanura.indicator(), -100, false);
    mount.incMk(Mount::DORAHYOU, 1, mKanura.indicator(), -100, false);
    mount.incMk(Mount::URAHYOU, 0, mKanura.indicator(), -100, false);
}

std::optional<HrhInitFix> Awai::onHrhBeg(util::Rand &rand, const TileCount &stock)
{
    if (!usingDaburii())
        return std::nullopt;

    HrhInitFix fix;

    // add 3 kanura
    TileCount myStock(stock);
    myStock.clear(mKanura);
    for (int i = 0; i < 3; i++)
        fix.targets.pushBack(T37(mKanura.id34()));

    // FUCK avoid dora
//    myStock.clear();

    begIter(fix, rand, myStock);

    return fix;
}

Girl::IrsCtrlGetter Awai::attachIrsOnDice()
{
    return &Awai::mIrsCtrl;
}

bool Awai::usingDaburii() const
{
    return mIrsCtrl.itemAt(0).on();
}

T34 Awai::kanuraOfPlan(int plan) const
{
    using namespace tiles34;
    static const std::array<T34, 24> noFives {
        1_m, 2_m, 3_m, 4_m, 6_m, 7_m, 8_m, 9_m,
        1_p, 2_p, 3_p, 4_p, 6_p, 7_p, 8_p, 9_p,
        1_s, 2_s, 3_s, 4_s, 6_s, 7_s, 8_s, 9_s,
    };

    return plan < 24 ? noFives[plan] : mSomeGuestWind;
}

int Awai::lastCorner(int dice, int kanCt)
{
    int tailRemain = 2 * dice;
    tailRemain -= 14;
    tailRemain -= kanCt;
    if (tailRemain <= 0)
        tailRemain += 34;

    return tailRemain;
}

void Awai::begIter(HrhInitFix &fix, util::Rand &rand, const TileCount &stock)
{
    util::Stactor<Suit, 3> avaiSuits;
    for (Suit s : { Suit::M, Suit::P, Suit::S })
        if (stock.has(s))
            avaiSuits.pushBack(s);

    // a significant but not necessary random generation
    // by this generation, the generated complete hand has no yaku, but
    // after 2 swap-out's it may have. we simply ignore such a case.
    int iter = 500;
    while (iter-- > 0) {
        // generate a sketch of winning hand
        InitSketch ske = sketch(rand, avaiSuits);

        if (!stock.covers(ske.need))
            continue;

        // pick the last wait tile
        if (!pickWait(fix, rand, ske, stock))
            continue;

        // done
        for (const T37 &t : ske.need.t37s13(true))
            fix.targets.pushBack(t);

        break;
    }
}

Awai::InitSketch Awai::sketch(util::Rand &rand, const util::Stactor<Suit, 3> &avaiSuits) const
{
    // (1) generate a yao sequence and a non-yao sequence,
    //     whose both suits and values are different.
    // (2) generate either a sequence or a number-triplet.
    //     while generating the sequence, intentionally avoid 1pk.
    // (3) generate a number-pair
    InitSketch res;

    // (1)
    int yaoSuitId = rand.gen(avaiSuits.size());
    Suit yaoS = avaiSuits[yaoSuitId];
    int yaoV = rand.gen(2) == 0 ? 1 : 7;
    T37 yaoSeq(yaoS, yaoV);
    Suit midS = avaiSuits[(yaoSuitId + 1) % avaiSuits.size()];
    int midV = 2 + rand.gen(5);
    T37 midSeq(midS, midV);
    res.heads[0] = yaoSeq;
    res.heads[1] = midSeq;
    res.need.inc(yaoSeq, 1);
    res.need.inc(T37(yaoSeq.next().id34()), 1);
    res.need.inc(T37(yaoSeq.nnext().id34()), 1);
    res.need.inc(midSeq, 1);
    res.need.inc(T37(midSeq.next().id34()), 1);
    res.need.inc(T37(midSeq.nnext().id34()), 1);

    // (2)
    if (rand.gen(3) > 0) { // 2/3 sequence
        T37 lastSeq(avaiSuits[rand.gen(avaiSuits.size())], rand.gen(7) + 1);
        // avoid 1pk by mapping 123567 to 765321, and 4 to 3
        if (lastSeq == yaoSeq || lastSeq == midSeq)
            lastSeq = T37(lastSeq.suit(), lastSeq.val() == 4 ? 3 : 8 - lastSeq.val());

        res.thridIsTri = false;
        res.heads[2] = lastSeq;
        res.need.inc(lastSeq, 1);
        res.need.inc(T37(lastSeq.next().id34()), 1);
        res.need.inc(T37(lastSeq.nnext().id34()), 1);
    } else {
        T37 lastTri(avaiSuits[rand.gen(avaiSuits.size())], rand.gen(9) + 1);
        res.thridIsTri = true;
        res.heads[2] = lastTri;
        res.need.inc(lastTri, 3);
    }

    // (3)
    T37 pair(avaiSuits[rand.gen(avaiSuits.size())], rand.gen(9) + 1);
    res.pair = pair;
    res.need.inc(pair, 2);

    return res;
}

bool Awai::pickWait(HrhInitFix &fix, util::Rand &rand, InitSketch &ske, const TileCount &stock)
{
    std::vector<T37> kickables;

    if (ske.thridIsTri) {
        // horizontally spread, avoid wait-almost-in-hand
        if (ske.need.ct(ske.heads[2]) == 3
            && ske.need.ct(ske.pair) == 2
            && stock.ct(T37(ske.heads[2].id34())) == 4
            && stock.ct(T37(ske.pair.id34())) == 4) {
            // init has at most one closed tri, looks comfortable
            kickables.push_back(T37(ske.heads[2].id34()));
        }
    } else {
        for (T34 head : ske.heads) {
            // *INDENT-OFF*
            auto waity = [&](T34 t) {
                // horizontally spread, avoid wait-almost-in-hand
                return ske.need.ct(t) == 1 && stock.ct(T37(t.id34())) >= 3;
            };
            // *INDENT-ON*
            if (waity(head.next())) // clamp
                kickables.push_back(T37(head.next().id34()));

            if (head.val() == 1 && waity(head.nnext())) // 12+3
                kickables.push_back(T37(head.nnext().id34()));

            if (head.val() == 7 && waity(head)) // 7+89
                kickables.push_back(T37(head.id34()));
        }
    }

    if (kickables.empty())
        return false;

    const T37 &kick3 = kickables[rand.gen(kickables.size())];
    ske.need.inc(kick3, -1);

    // check if the kanura is ankanable after riichi
    ske.need.inc(T37(mKanura.id34()), 3);
    bool ankanable = ske.need.onlyInTriplet(mKanura, 0);
    ske.need.inc(T37(mKanura.id34()), -3);
    if (!ankanable) {
        ske.need.inc(kick3, 1); // recover, useless if see caller, though
        return false;
    }

    mLastWaits.clear();
    if (ske.thridIsTri) { // bibump wait
        T37 p37(ske.pair.id34());
        fix.loads.emplaceBack(kick3, 2); // reserve other 2 for initPopExact
        fix.loads.emplaceBack(p37, 2);
        mLastWaits.push_back(kick3);
        mLastWaits.push_back(p37);
    } else {
        fix.loads.emplaceBack(kick3, 4); // the more, the better
        mLastWaits.push_back(kick3);
    }

    // make step-1 (or step-0 if lucky)
    util::Stactor<T37, 13> kick1ables = ske.need.t37s13();
    mNeedFirstDraw = true;
    mFirstDraw = kick1ables[rand.gen(kick1ables.size())];
    ske.need.inc(mFirstDraw, -1);
    fix.loads.emplaceBack(mFirstDraw, 1);

    return true;
}



} // namespace saki
