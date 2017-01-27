#include "girls_shiraitodai.h"
#include "princess.h"
#include "table.h"
#include "util.h"

#include <algorithm>



namespace saki
{



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

void Takami::nonMonkey(Rand &rand, TileCount &init, Mount &mount,
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
    for (int ti = 0; ti < 34; ti++)
        if (init.closed().ct(T34(ti)) >= 3)
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
    const std::vector<M37> &barks = hand.barks();

    static const auto isFish = [](const M37 &m) {
        return m.type() == M37::Type::PON
                || m.type() == M37::Type::DAIMINKAN
                || m.type() == M37::Type::KAKAN;
    };
    int fishCt = std::count_if(barks.begin(), barks.end(), isFish);

    if (fishCt >= 3) {
        accelerate(mount, hand, table.getRiver(mSelf), ACCEL_MK);
    } else if (!hand.ready()) {
        int needPair = 4 - fishCt;
        for (int ti = 0; ti < 34; ti++) {
            T34 t(ti);
            if (closed.ct(t)) {
                needPair--;
                mount.lightA(t, -PAIR_MK);
            }
        }
        if (needPair > 0) {
            for (T34 t : tiles34::YAO13) {
                if (closed.ct(t) != 1)
                    continue;
                bool noPrec = t.isZ() || t.val() == 1 || closed.ct(t.prev()) == 0;
                bool noSucc = t.isZ() || t.val() == 9 || closed.ct(t.next()) == 0;
                if (noPrec && noSucc)
                    mount.lightA(t, PAIR_MK);
            }
        }
    }
}



void Awai::onDice(Rand &rand, const Table &table, TicketFolder &tickets)
{
    (void) rand;
    (void) table;
    mTicketsBackup = tickets;
    tickets = TicketFolder(ActCode::IRS_CHECK);
}

bool Awai::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who == mSelf)
        return true;

    if (iter > 500) {
        util::p("awai-rival iter over 500");
        return true;
    }

    return init.step() >= (iter > 300 ? 4 : 5);
}

void Awai::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    if (!mDaburi.on)
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
        std::vector<T34> effs = table.getHand(who).effA();
        for (T34 t : effs)
            mount.lightA(t, -ACCEL_MK);
        return;
    }

    if (rinshan || !mDaburi.on)
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
        const std::vector<M37> &barks = table.getHand(mSelf).barks();
        int barkCt = barks.size();
        if (barkCt > 0
                && util::any(barks, [](const M37 &m) { return m.type() == M37::Type::ANKAN; })
                && mount.wallRemain() <= lastCorner(table.getDice())) {
            // after kan and corner, for every player,
            // make wait-tile appear in mount continuously
            for (int ti = 0; ti < 34; ti++) {
                T34 t(ti);
                mount.lightA(t, -EJECT_MK);
                mount.lightB(t, t == mLastWait ? ACCEL_MK : -EJECT_MK);
            }
        } else if (who == mSelf) {
            // before kan
            int tail = lastCorner(table.getDice());
            int rem = mount.wallRemain();
            if (tail < rem && rem <= tail + 4) {
                // before last corner, extract kan-material
                for (int ti = 0; ti < 34; ti++) {
                    T34 t(ti);
                    mount.lightA(t, -EJECT_MK);
                    mount.lightB(t, t == mKanura ? DRAG_MK : -EJECT_MK);
                }
            } else {
                // don't tsumo too early
                accelerate(mount, table.getHand(mSelf), std::vector<T37>(), -EJECT_MK);
                // don't get the kan-material which is not kanura
                // (awai can have 2 closed triplets in init hand)
                for (int ti = 0; ti < 34; ti++)
                    if (table.getHand(mSelf).closed().ct(T34(ti)) == 3)
                        mount.lightA(T34(ti), -EJECT_MK);
            }
        }
    }
}

void Awai::nonMonkey(Rand &rand, TileCount &init, Mount &mount,
                     std::bitset<Girl::NUM_NM_SKILL> &presence,
                     const Princess &princess)
{
    if (!mDaburi.on)
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
    // (1) generate a yao sequence and a non-yao sequence,
    //     whose both suits and values are different.
    // (2) generate either a sequence or a number-triplet.
    //     while generating the sequence, intentionally avoid 1pk.
    // (3) generate a number-pair
    // (4) check if the mount affords this generated hand, and none of
    //     the generated tiles may be a dora. return to (1) until satisfied.
    // (5) check if the kanura is ankanable after riichi
    // (6) kick out 2 tiles in the generation to make it a 1-step hand.
    //     one of the 2 kicked out tile must have 3 in stoch A such that
    //     it will appear after the corner continously.
    // by this generation, the generated complete hand has no yaku, but
    // after 2 swap-out's it may have. we simply ignore such a case.
    int iter = 500;
    while (iter --> 0) {
        // (1)
        int yaoSuitId = rand.gen(avaiSuits.size());
        Suit yaoS = avaiSuits[yaoSuitId];
        int yaoV = rand.gen(2) == 0 ? 1 : 7;
        T37 yaoSeq(yaoS, yaoV);
        Suit midS = avaiSuits[(yaoSuitId + 1) % avaiSuits.size()];
        int midV = 2 + rand.gen(5);
        T37 midSeq(midS, midV);
        TileCount need;
        need.inc(yaoSeq, 1);
        need.inc(T37(yaoSeq.next().id34()), 1);
        need.inc(T37(yaoSeq.nnext().id34()), 1);
        need.inc(midSeq, 1);
        need.inc(T37(midSeq.next().id34()), 1);
        need.inc(T37(midSeq.nnext().id34()), 1);

        // (2)
        if (rand.gen(3) > 0) { // 66% sequence
            T37 lastSeq(avaiSuits[rand.gen(avaiSuits.size())], rand.gen(7) + 1);
            // avoid 1pk by mapping 123567 to 765321, and 4 to 3
            if (lastSeq == yaoSeq || lastSeq == midSeq)
                lastSeq = T37(lastSeq.suit(), lastSeq.val() == 4 ? 3 : 8 - lastSeq.val());
            need.inc(lastSeq, 1);
            need.inc(T37(lastSeq.next().id34()), 1);
            need.inc(T37(lastSeq.nnext().id34()), 1);
        } else {
            T37 lastTri(avaiSuits[rand.gen(avaiSuits.size())], rand.gen(9) + 1);
            need.inc(lastTri, 3);
        }

        // (3)
        T37 pair(avaiSuits[rand.gen(avaiSuits.size())], rand.gen(9) + 1);
        need.inc(pair, 2);

        // (4)
        if (!mount.affordA(need))
            continue;

        auto mayHaveDora = [&princess](T34 t) { return princess.mayHaveDora(t); };
        if (iter > 100 && util::any(need.t34s(), mayHaveDora))
            continue;

        // (5)
        need.inc(T37(mKanura.id34()), 3);
        if (!need.onlyInTriplet(mKanura, 1))
            continue;
        need.inc(T37(mKanura.id34()), -3);

        // (6)
        std::vector<T37> kick3ables;
        for (const T37 &kick : need.t37s())
            if (need.ct(kick) == 1 && mount.remainA(kick) >= 3)
                kick3ables.push_back(kick);

        if (kick3ables.empty())
            continue;

        const T37 &kick3 = kick3ables[rand.gen(kick3ables.size())];
        need.inc(kick3, -1);
        mount.loadB(kick3, 4); // as many as possible
        mLastWait = kick3;

        std::vector<T37> kick1ables = need.t37s();
        mNeedFirstDraw = true;
        mFirstDraw = kick1ables[rand.gen(kick1ables.size())];
        need.inc(mFirstDraw, -1);

        // done
        mount.loadB(mFirstDraw, 1);
        for (const T37 &t : need.t37s(true))
            init.inc(mount.initPopExact(t), 1);
        break;
    }

    if (iter == -1)
        util::p("awai-daburi iter over");
}

const IrsCheckRow &Awai::irsCheckRow(int index) const
{
    assert(index == 0);
    return mDaburi;
}

int Awai::irsCheckCount() const
{
    return 1;
}

TicketFolder Awai::forwardAction(const Table &table, Mount &mount, const Action &action)
{
    (void) table;
    (void) mount;
    mDaburi.on = action.mask() & 0b1;
    return mTicketsBackup;
}

int Awai::lastCorner(int dice)
{
    int tailRemain = 2 * dice;
    tailRemain -= 14;
    if (tailRemain <= 0)
        tailRemain += 34;
    return tailRemain;
}



} // namespace saki


