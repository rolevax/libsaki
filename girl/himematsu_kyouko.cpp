#include "himematsu_kyouko.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/dismember.h"
#include "../util/misc.h"



namespace saki
{



bool Kyouko::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    if (who != mSelf)
        return true;

    if (iter > 30) {
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
        util::Stactor<T34, 34> myEffs = table.getHand(mSelf).effA4(); // exclude 7/13
        util::Stactor<T34, 34> herEffs = table.getHand(who).effA();

        if (mCourse == TANYAO)
            std::remove_if(myEffs.begin(), myEffs.end(), PredThis(&T34::isYao));

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
