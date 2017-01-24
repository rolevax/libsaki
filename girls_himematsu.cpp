#include "girls_himematsu.h"
#include "table.h"
#include "princess.h"
#include "myrand.h"
#include "util.h"

#include <algorithm>
#include <cassert>
#include <iostream>



namespace saki
{



const std::array<int, 4> Suzu::POWERS { 80, 130, 230, 330 };

void Suzu::onDice(const Table &table, TicketFolder &tickets)
{
    (void) tickets;

    if (!mExploded) {
        int rand = myRand() % 100;
        int remainRound = table.getRuleInfo().roundLimit - table.getRound();
        int threshold = remainRound < 4 ? 20 : 10;
        if (rand < threshold) {
            mExploded = true;
            table.popUp(mSelf);
        }
    }
}

void Suzu::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;

    if (!mExploded)
        return;

    for (Suit s : { Suit::M, Suit::P, Suit::S })
        for (int i = 0; i < 4; i++)
            exists[mSelf.index()].inc(T34(s, 6 + i), POWERS[i]);

    for (T34 t : tiles34::Z7)
        exists[mSelf.index()].inc(t, Z_POWER);
}

bool Suzu::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who != mSelf || iter > 10)
        return true;

    // no quad
    for (int ti = 0; ti < 34; ti++)
        if (init.closed().ct(T34(ti)) == 4)
            return false;

    return true;
}

void Suzu::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || !mExploded || rinshan)
        return;

    const TileCount &closed = table.getHand(mSelf).closed();

    for (Suit s : { Suit::M, Suit::P, Suit::S }) {
        for (int i = 0; i < 4; i++) {
            T34 t(s, 6 + i);
            if (closed.ct(t) < 3)
                mount.lightA(t, POWERS[i]);
        }
    }

    for (T34 t : tiles34::Z7)
        if (closed.ct(t) < 3)
            mount.lightA(t, Z_POWER);
}

std::string Suzu::popUpStr() const
{
    return std::string("DUANG");
}




bool Kyouko::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    if (who != mSelf)
        return true;

    if (iter > 30) {
        util::p("Kyouko iter over 30");
        mCourse = NONE;
        return true;
    }

    if (init.step() > 2)
        return false;

    if (init.closed().ctYao() < 3) {
        mCourse = TANYAO;
        return true;
    }

    using namespace tiles34;
    const Table &table = princess.getTable();
    T34 sw(Suit::F, table.getSelfWind(mSelf));
    T34 rw(Suit::F, table.getRoundWind());
    std::array<T34, 5> yakuhais { 1_y, 2_y, 3_y, sw, rw };
    if (util::any(yakuhais, [&init](T34 y) { return init.closed().ct(y) >= 2; })) {
        mCourse = YAKUHAI;
        return true;
    }

    return false;
}

void Kyouko::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (rinshan || mCourse == NONE)
        return;

    if (table.getRiver(mSelf).size() > 6)
        return;

    const Hand &myHand = table.getHand(mSelf);

    if (who == mSelf) {
        if (myHand.ready())
            accelerate(mount, myHand, table.getRiver(mSelf), 300);
    } else {
        std::vector<T34> myEffs = table.getHand(mSelf).effA4(); // exclude 7/13
        std::vector<T34> herEffs = table.getHand(who).effA();

        if (mCourse == TANYAO) {
            std::remove_if(myEffs.begin(), myEffs.end(),
                           [](T34 t) { return t.isYao(); });
        }

        for (T34 t : myEffs) {
            if (who == mSelf.left() && myHand.canChii(t)) {
                if (!util::has(herEffs, t))
                    mount.lightA(t, 1000);
            } else if (myHand.canPon(t)) {
                if (!util::has(herEffs, t))
                    mount.lightA(t, 1000);
            }
        }
    }
}



} // namespace saki


