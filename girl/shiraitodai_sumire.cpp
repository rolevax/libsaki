#include "shiraitodai_sumire.h"
#include "../form/parsed_view.h"
#include "../table/princess.h"
#include "../table/table.h"
#include "../table/table_view_hand.h"
#include "../ai/ai.h"
#include "../util/misc.h"

#include <sstream>



namespace saki
{



void Sumire::onDice(util::Rand &rand, const Table &table)
{
    (void) rand;
    (void) table;

    mTarget = Who();
}

bool Sumire::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    if (who != mSelf || iter > 100)
        return true;

    int sw = table.getSelfWind(mSelf);
    int rw = table.getRoundWind();
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

    if (hand.ready() && hand.hasEffA(mWant))
        return;

    closeUpToPrey(table, mount, rinshan);
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

void Sumire::closeUpToPrey(const Table &table, Mount &mount, bool rinshan)
{
    const Hand &hand = table.getHand(mSelf);

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

    if (hand.step7() == 0) {
        util::p("TODO case: 7 pair case, just swap");
    } if (mWant.isYao()) {
        util::p("TODO case: mWant is yao");
        // FUCK
    } else {
        // clamp
        Parsed4s parses = hand.parse4();
        if (parses.step4() == 0) {
            for (const auto &parse : parses) {
                ParsedView4Ready view(parse);
                if (std::optional<T34> isorider = view.getIsorider()) {
                    for (T34 t : tiles34::ALL34)
                        mount.lightA(t, t == *isorider ? 100 : -40, rinshan);
                } else {
                    util::Stactor<C34, 2> comelds = view.get2s();
                    auto need = [&](T34 t) {
                        auto has =  [t](const C34 &c) { return c.has(t); };
                        bool related = (t | mWant) || (mWant | t);
                        return related && util::none(comelds, has);
                    };

                    for (T34 t : tiles34::ALL34)
                        mount.lightA(t, need(t) ? 100 : -40, rinshan);
                }
            }
        } else if (parses.step4() == 1) {
            // FUCK must be in 3 meld + 1 pair + 2 float form,
            //      otherwise the user is an SB, and just ignore
        } else { // step4 > 1
            recoverReady();
        }
    }
}

void Sumire::shapeYaku(const Table &table, Mount &mount, bool rinshan)
{
    const Hand &hand = table.getHand(mSelf);

    if (!hand.isMenzen())
        return;

    if (shapeYakuhai(table, mount, rinshan))
        return;

    shapeTanyao(table, mount, rinshan);

    // TODO 3sk and 1pk (?)
}

bool Sumire::shapeYakuhai(const Table &table, Mount &mount, bool rinshan)
{
    const Hand &hand = table.getHand(mSelf);

    int sw = table.getSelfWind(mSelf);
    int rw = table.getRoundWind();
    auto isPluralYakuhai = [&](T34 z) {
        return z.isYakuhai(sw, rw) && hand.closed().ct(z) >= 2;
    };

    const auto &z7 = tiles34::Z7;
    if (util::none(z7, isPluralYakuhai))
        return false;

    for (T34 z : z7)
        if (hand.closed().ct(z) == 2)
            mount.lightA(z, 500, rinshan);

    return true;
}

bool Sumire::shapeTanyao(const Table &table, Mount &mount, bool rinshan)
{
    const Hand &hand = table.getHand(mSelf);

    if (hand.closed().ctYao() > 2)
        return false;

    for (int v = 3; v <= 7; v++)
        for (Suit s : { Suit::M, Suit::P, Suit::S })
            if (hand.hasEffA(T34(s, v)))
                mount.lightA(T34(s, v), 100, rinshan);

    return true;
}



} // namespace saki
