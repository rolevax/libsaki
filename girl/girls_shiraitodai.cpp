#include "girls_shiraitodai.h"
#include "../table/princess.h"
#include "../table/table.h"
#include "../table/table_view_hand.h"
#include "../ai/ai.h"
#include "../util/misc.h"

#include <sstream>



namespace saki
{



void Teru::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) exists;
    (void) princess;
    // TODO implement
}

void Teru::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;
    (void) mount;
    (void) rinshan;

    if (who != mSelf)
        return;

    // TODO implement
}

void Teru::onRoundEnded(const Table &table, RoundResult result,
                        const std::vector<Who> &openers, Who gunner,
                        const std::vector<Form> &fs)
{
    (void) table;
    (void) gunner;

    if (result == RoundResult::TSUMO && openers[0] == mSelf) {
        mPrevGain = fs[0].netGain();
    } else if (result == RoundResult::RON) {
        if (openers[0] == mSelf)
            mPrevGain = fs[0].netGain();
        else if (fs.size() == 2 && openers[1] == mSelf)
            mPrevGain = fs[1].netGain();
    } else {
        mPrevGain = 0;
    }
}

void Teru::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                     std::bitset<Girl::NUM_NM_SKILL> &presence,
                     const Princess &princess)
{
    (void) rand;
    (void) init;
    (void) mount;
    (void) presence; // TODO care about zim and kuro
    (void) princess;
    // TODO implement
}



void Sumire::onDice(util::Rand &rand, const Table &table)
{
    (void) rand;
    (void) table;

    mTarget = Who();
}

bool Sumire::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who != mSelf || iter > 100)
        return true;

    int sw = princess.getTable().getSelfWind(mSelf);
    int rw = princess.getTable().getRoundWind();
    if (iter < 5)
        for (T34 z : tiles34::Z7)
            if (z.isYakuhai(sw, rw) && init.closed().ct(z) >= 2)
                return true;

    const auto &ts = init.closed().t34s13();
    auto isYao = [](T34 t) { return t.isYao(); };
    return std::count_if(ts.begin(), ts.end(), isYao) <= 3;
}

void Sumire::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who == mSelf)
        handleDrawSelf(table, mount, rinshan);
    else if (mTarget.somebody() && who == mTarget)
        handleDrawTarget(table, mount, rinshan);
}

void Sumire::onIrsChecked(const Table &table, Mount &mount)
{
    if (mIrsCtrl.itemAt(MAIN).on()) {
        if (mIrsCtrl.itemAt(RIGHT).on())
            mTarget = mSelf.right();
        else if (mIrsCtrl.itemAt(CROSS).on())
            mTarget = mSelf.cross();
        else if (mIrsCtrl.itemAt(LEFT).on())
            mTarget = mSelf.left();
    }

    if (mTarget.somebody())
        pickBullet(table, mount);
}

std::string Sumire::popUpStr() const
{
    std::ostringstream oss;
    oss << ">> " << mWant << " <<";
    return oss.str();
}

Girl::IrsCtrlGetter Sumire::attachIrsOnDrawn(const Table &table)
{
    if (aimable(table)) {
        mIrsCtrl.setClickHost(table.getChoices(mSelf));
        return &Sumire::mIrsCtrl;
    }

    return nullptr;
}

void Sumire::handleDrawSelf(const Table &table, Mount &mount, bool rinshan)
{
    const Hand &hand = table.getHand(mSelf);
    if (mTarget.nobody()) {
        if (!hand.ready())
            shapeYaku(table, mount, rinshan);

        return;
    }

    // *INDENT-OFF*
    auto recoverReady = [&]() {
        for (T34 t : tiles34::ALL34) {
            auto equal = [t](const T37 &r) { return t == r; };
            const auto &river = table.getRiver(mSelf);
            bool need = t != mWant && hand.hasEffA(t) && util::none(river, equal);
            mount.lightA(t, need ? 100 : -40, rinshan);
        }
    };
    // *INDENT-ON*

    const TileCount &closed = hand.closed();
    if (mWant.isYao()) {
        // isoride
        if (closed.ct(mWant) == 0) {
            for (T34 t : tiles34::ALL34) {
                mount.lightA(t, -40, rinshan);
                mount.lightB(t, t == mWant ? 100 : -40, rinshan);
            }
        } else if (hand.step() > 0) {
            recoverReady();
        }
    } else {
        // clamp
        if (closed.ct(mWant.prev()) == 0) {
            for (T34 t : tiles34::ALL34) {
                mount.lightA(t, -40, rinshan);
                mount.lightB(t, t | mWant ? 100 : -40, rinshan);
            }
        } else if (closed.ct(mWant.next()) == 0) {
            for (T34 t : tiles34::ALL34) {
                mount.lightA(t, -40, rinshan);
                mount.lightB(t, mWant | t ? 100 : -40, rinshan);
            }
        } else {
            recoverReady();
        }
    }
}

void Sumire::handleDrawTarget(const Table &table, Mount &mount, bool rinshan)
{
    if (shootable(table)) {
        // feed
        if (mShootTrial == 0) {
            for (T34 t : tiles34::ALL34) {
                mount.lightA(t, -500, rinshan);
                mount.lightB(t, t == mFeed ? 100 : -100, rinshan);
            }

            mShootTrial++;
        } else if (mShootTrial == 1) {
            // TODO re-aim logic
            mShootTrial++;
        }
    } else {
        // block
        for (T34 t : tiles34::ALL34)
            if (table.getHand(mTarget).hasEffA(t))
                mount.lightA(t, -40, rinshan);
    }
}

bool Sumire::aimable(const Table &table)
{
    if (mTarget.somebody())
        return false;

    for (int w = 0; w < 4; w++) {
        Who who(w);
        const Hand &hand = table.getHand(who);

        if (who == mSelf) {
            bool dama = hand.isMenzen() && hand.ready()
                && !table.riichiEstablished(mSelf);
            if (!dama)
                return false;
        } else {
            if (hand.ready())
                return false; // must be the first readyer

        }
    }

    return true;
}

bool Sumire::shootable(const Table &table)
{
    if (mTarget.nobody())
        return false;

    const Hand &myHand = table.getHand(mSelf);
    bool dama = myHand.isMenzen() && myHand.ready()
        && !table.riichiEstablished(mSelf);
    if (!dama)
        return false;

    return myHand.hasEffA(mWant);
}

void Sumire::pickBullet(const Table &table, Mount &mount)
{
    int maxHappy = 0;
    mWant = table.getHand(mSelf).effA().front();

    const Hand &taragetHand = table.getHand(mTarget);
    const Hand &myHand = table.getHand(mSelf);

    // *INDENT-OFF*
    auto niceMount = [&myHand, &mount](const T37 feed) {
        if (mount.remainA(feed) == 0)
            return false;

        T34 feed34(feed);
        if (feed34.isYao()) {
            int got = myHand.closed().ct(feed34);
            int fetchPlusFeed = mount.remainA(feed34);
            return got + fetchPlusFeed >= 2; // can form a pair
        } else {
            bool gotL = myHand.closed().ct(feed34.prev()) > 0;
            bool gotR = myHand.closed().ct(feed34.next()) > 0;

            // consider only black tiles to kill all headache
            bool canGetL = mount.remainA(T37(feed34.prev().id34())) > 0;
            bool canGetR = mount.remainA(T37(feed34.next().id34())) > 0;

            return (gotL || canGetL) && (gotR || canGetR);
        }
    };
    // *INDENT-ON*

    for (const T37 &feed : tiles37::ORDER37) {
        if (!niceMount(feed))
            continue;

        Hand dream = taragetHand; // copy
        dream.draw(feed);
        TableViewHand view(table, dream, mTarget);

        Action act = Ai::thinkStdDrawnAttack(view);
        if (act.isDiscard() || act.isRiichi()) {
            act = act.toDiscard();
            int step = dream.peekDiscard(act, &Hand::step);
            auto effA = dream.peekDiscard(act, &Hand::effA);
            int effACt = table.visibleRemain(mTarget).ct(effA);
            int doraCt = mount.getDrids() % dream + dream.ctAka5();
            int happy = 100 * (13 - step) + 3 * effACt + doraCt;
            if (happy > maxHappy) {
                mWant = act.act() == ActCode::SWAP_OUT ? act.t37() : dream.drawn();
                mFeed = feed;
                maxHappy = happy;
            }
        }
    }

    if (maxHappy > 0) {
        mount.loadB(mFeed, 1);

        // reserve isorider or clamper
        if (mWant.isYao()) { // isoride
            if (myHand.closed().ct(mWant) == 0)
                mount.loadB(T37(mWant.id34()), 1);
        } else { // clamp
            if (myHand.closed().ct(mWant.prev()) == 0)
                mount.loadB(T37(mWant.prev().id34()), 1);

            if (myHand.closed().ct(mWant.next()) == 0)
                mount.loadB(T37(mWant.next().id34()), 1);
        }

        table.popUp(mSelf);
    }
}

void Sumire::shapeYaku(const Table &table, Mount &mount, bool rinshan)
{
    const Hand &hand = table.getHand(mSelf);

    if (!hand.isMenzen())
        return;

    // yakuhai
    int sw = table.getSelfWind(mSelf);
    int rw = table.getRoundWind();
    for (T34 z : tiles34::Z7) {
        if (z.isYakuhai(sw, rw) && hand.closed().ct(z) == 2) {
            mount.lightA(z, 500, rinshan);
            return;
        }
    }

    // tanyao
    if (hand.closed().sum() - hand.closed().ctYao() <= 2)
        for (int v = 3; v <= 7; v++)
            for (Suit s : { Suit::M, Suit::P, Suit::S })
                mount.lightA(T34(s, v), 100, rinshan);

    // TODO 3sk and 1pk (?)
}



void Takami::onDiscarded(const Table &table, Who who)
{
    if (who != mSelf)
        return;

    if (table.getRiver(mSelf).size() == 1) {
        const T37 &last = table.getFocusTile();
        // ignore if over-4
        if (std::count(mSlots.begin(), mSlots.end(), last) < 4)
            mSlots.push_back(last);
    }
}

void Takami::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                       std::bitset<Girl::NUM_NM_SKILL> &presence,
                       const Princess &princess)
{
    (void) rand;
    (void) presence;
    (void) princess;

    if (!princess.getTable().isAllLast())
        return;

    // assume seed slots has no vertical overflow
    // assume Takami::nonMonkey() is executed before any other
    // non-monkey such that initPopExact always works
    int slotCt = std::min(13, static_cast<int>(mSlots.size()));
    for (int i = 0; i < slotCt; i++) {
        T37 t = mSlots[i];
        if (mount.remainA(t) == 0)
            t = t.toInverse5();

        init.inc(mount.initPopExact(t), 1);
    }
}

int Takami::d3gNeed(T34 t) const
{
    if (t.suit() != Suit::Y)
        return 0;

    T37 t37(t.id34());
    int ct = std::count(mSlots.begin(), mSlots.end(), t37);
    return ct >= 3 ? 0 : 3 - ct;
}



bool Seiko::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who != mSelf || iter > 100)
        return true;

    // no triplet
    for (T34 t : tiles34::ALL34)
        if (init.closed().ct(t) >= 3)
            return false;

    int yaoPairCt = std::count_if(tiles34::YAO13.begin(), tiles34::YAO13.end(),
                                  [&init](T34 t) { return init.closed().ct(t) == 2; });
    return yaoPairCt >= 2;
}

void Seiko::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(mSelf);
    const TileCount &closed = hand.closed();
    const auto &barks = hand.barks();

    // *INDENT-OFF*
    const auto isFish = [](const M37 &m) {
        return m.type() == M37::Type::PON
                || m.type() == M37::Type::DAIMINKAN
                || m.type() == M37::Type::KAKAN;
    };
    // *INDENT-ON*
    int fishCt = std::count_if(barks.begin(), barks.end(), isFish);

    if (fishCt >= 3) {
        accelerate(mount, hand, table.getRiver(mSelf), ACCEL_MK);
    } else if (!hand.ready()) {
        int needPair = 4 - fishCt;
        for (T34 t : tiles34::ALL34) {
            if (closed.ct(t) == 2) {
                needPair--;
                mount.lightA(t, -PAIR_MK);
            }
        }

        if (needPair > 0) {
            using namespace tiles34;
            for (T34 t : YAO13)
                if (closed.ct(t) == 1)
                    mount.lightA(t, PAIR_MK);

            for (T34 t : { 2_m, 8_m, 2_p, 8_p, 2_s, 8_s })
                if (closed.ct(t) == 1)
                    mount.lightA(t, PAIR_MK);
        }
    }
}

bool Awai::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who == mSelf || iter > 500)
        return true;

    return init.step() >= (iter > 300 ? 4 : 5);
}

void Awai::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    if (!mIrsCtrl.itemAt(0).on())
        return;

    using Indic = Princess::Indic;
    int self = mSelf.index();
    T34 dora = princess.getImageIndic(Indic::DORA).dora();
    T34 ura = princess.getImageIndic(Indic::URADORA).dora();
    T34 kandora = princess.getImageIndic(Indic::KANDORA).dora();

    exists[self].inc(dora, -EJECT_MK);
    exists[self].inc(ura, -EJECT_MK);
    exists[self].inc(kandora, -EJECT_MK);
    // kanura has been loaded to B-space (see nonMonkey())

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

    if (rinshan || !mIrsCtrl.itemAt(0).on())
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

void Awai::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                     std::bitset<Girl::NUM_NM_SKILL> &presence,
                     const Princess &princess)
{
    if (!mIrsCtrl.itemAt(0).on())
        return;

    // add 3 kanura
    mKanura = princess.getImageIndic(Princess::Indic::KANURA).dora();
    for (int i = 0; i < 3; i++)
        init.inc(mount.initPopExact(T37(mKanura.id34())), 1);

    mount.loadB(T37(mKanura.id34()), 1);

    // exclude ZIM-suit from init hand
    std::vector<Suit> avaiSuits { Suit::M, Suit::P, Suit::S };
    if (presence.test(ZIM_M))
        avaiSuits.erase(avaiSuits.begin());
    else if (presence.test(ZIM_P))
        avaiSuits.erase(avaiSuits.begin() + 1);
    else if (presence.test(ZIM_S))
        avaiSuits.erase(avaiSuits.begin() + 2);

    // a significant but not necessary random generation
    // by this generation, the generated complete hand has no yaku, but
    // after 2 swap-out's it may have. we simply ignore such a case.
    int iter = 500;
    while (iter-- > 0) {
        // generate a sketch of winning hand
        InitSketch ske = sketch(rand, avaiSuits);

        // check if mount affords and hand doraless
        if (!mount.affordA(ske.need))
            continue;

        auto mayHaveDora = [&princess](T34 t) { return princess.mayHaveDora(t); };
        if (iter > 100 && util::any(ske.need.t34s13(), mayHaveDora))
            continue;

        // pick the last wait tile
        if (!pickWait(rand, ske, mount))
            continue;

        // done
        for (const T37 &t : ske.need.t37s13(true))
            init.inc(mount.initPopExact(t), 1);

        break;
    }
}

Girl::IrsCtrlGetter Awai::attachIrsOnDice()
{
    return &Awai::mIrsCtrl;
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

Awai::InitSketch Awai::sketch(util::Rand &rand, const std::vector<Suit> &avaiSuits) const
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

bool Awai::pickWait(util::Rand &rand, Awai::InitSketch &ske, Mount &mount)
{
    std::vector<T37> kickables;

    if (ske.thridIsTri) {
        // horizontally spread, avoid wait-almost-in-hand
        if (ske.need.ct(ske.heads[2]) == 3
            && ske.need.ct(ske.pair) == 2
            && mount.remainA(T37(ske.heads[2].id34())) == 4
            && mount.remainA(T37(ske.pair.id34())) == 4) {
            // init has at most one closed tri, looks comfortable
            kickables.push_back(T37(ske.heads[2].id34()));
        }
    } else {
        for (T34 head : ske.heads) {
            // *INDENT-OFF*
            auto waity = [&](T34 t) {
                // horizontally spread, avoid wait-almost-in-hand
                return ske.need.ct(t) == 1 && mount.remainA(T37(t.id34())) >= 3;
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
        mount.loadB(kick3, 2); // reserve other 2 for initPopExact
        mount.loadB(p37, 2);
        mLastWaits.push_back(kick3);
        mLastWaits.push_back(p37);
    } else {
        mount.loadB(kick3, 4); // the more, the better
        mLastWaits.push_back(kick3);
    }

    // make step-1 (or step-0 if lucky)
    util::Stactor<T37, 13> kick1ables = ske.need.t37s13();
    mNeedFirstDraw = true;
    mFirstDraw = kick1ables[rand.gen(kick1ables.size())];
    ske.need.inc(mFirstDraw, -1);
    mount.loadB(mFirstDraw, 1);

    return true;
}



} // namespace saki
