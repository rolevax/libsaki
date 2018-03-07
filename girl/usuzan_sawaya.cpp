#include "usuzan_sawaya.h"
#include "../table/table.h"
#include "../util/misc.h"



namespace saki
{

bool Sawaya::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who != mSelf || iter > 100 || !usingCloud(Cloud::WHITE))
        return true;

    return init.closed().ctS() >= 8;
}

void Sawaya::onDice(util::Rand &rand, const Table &table)
{
    (void) rand;
    (void) table;

    mPredice = true;
    mPaKorTarget = Who();

    for (int i = 0; i < Cloud::NUM_CLOUD; i++) {
        const IrsCheckItem &item = mCloudCtrl.itemAt(i);
        if (!item.mono() && !item.able())
            mCloudCtrl.setOnAt(i, false);
    }

    for (int i = 0; i < Kamuy::NUM_KAMUY; i++) {
        const IrsCheckItem &item = mKamuyCtrl.itemAt(i);
        if (!item.mono() && !item.able()) {
            if (i == Kamuy::HURI) {
                if (!mConsumedSecondHuri) {
                    mConsumedSecondHuri = true;
                    mKamuyCtrl.setOnAt(i, true);
                } else {
                    mKamuyCtrl.setOnAt(i, false);
                }
            } else {
                mKamuyCtrl.setOnAt(i, false); // TODO off hoyaw-kamuy by resist-time
            }
        }
    }
}

void Sawaya::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;

    mPredice = false;

    if (usingCloud(Cloud::RED) && usingCloud(Cloud::RED_RIVALS)) {
        for (T34 t : tiles34::Z7)
            for (int w = 0; w < 4; w++)
                exists[w].inc(t, w == mSelf.index() ? 100 : -1000);
    } else if (usingCloud(Cloud::RED) && usingCloud(Cloud::RED_SELF)) {
        for (T34 t : tiles34::Z7)
            exists[mSelf.index()].inc(t, -1000);
    }

    if (usingCloud(Cloud::WHITE))
        for (int i = 18; i < 27; i++) // bamboo
            exists[mSelf.index()].inc(T34(i), 70);
}

void Sawaya::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who == mPaKorTarget) {
        // pa-kor-kamuy, crazily give cannons
        const Hand &hand = table.getHand(mSelf);
        for (T34 t : tiles34::ALL34) {
            int mk = hand.hasEffA(t) ? 1000 : -50;
            mount.lightA(t, mk, rinshan);
            mount.lightB(t, mk, rinshan);
        }

        return; // priority higher than clouds and other kamuy
    }

    if (usingKamuy(Kamuy::AT_KOR)) {
        if (who == mSelf) {
            for (T34 t : tiles34::ALL34) {
                using namespace tiles34;
                // drag M or 3Y
                int mk = t.suit() == Suit::M || t == 3_y ? 1000 : -50;
                mount.lightA(t, mk, rinshan);
            }
        } else { // rivals' hand
            const Hand &hand = table.getHand(who);
            for (T34 t : tiles34::ALL34) {
                // drag useless M
                int mk = t.suit() == Suit::M && !hand.hasEffA(t) ? 1000 : -50;
                mount.lightA(t, mk, rinshan);
            }
        }
    }

    if (who == mSelf && usingKamuy(Kamuy::HURI)) {
        for (int i = 1; i <= 4; i++) {
            if (i == table.getSelfWind(mSelf))
                continue;

            T34 t(Suit::F, i);
            const TileCount &closed = table.getHand(mSelf).closed();
            if (closed.ct(t) == 1)
                mount.lightA(t, 1000, rinshan);
        }
    }

    if (usingCloud(Cloud::RED) && usingCloud(Cloud::RED_RIVALS)) {
        if (who == mSelf) {
            int mk = table.getHand(mSelf).ready() ? 40 : 60;
            for (T34 t : tiles34::Z7) {
                mount.lightB(t, mk / 2, rinshan);
                mount.lightA(t, mk, rinshan);
            }
        } else {
            for (T34 t : tiles34::Z7)
                mount.lightA(t, -1000, rinshan);
        }
    } else if (usingCloud(Cloud::RED) && usingCloud(Cloud::RED_SELF)) {
        if (who == mSelf)
            for (T34 t : tiles34::Z7)
                mount.lightA(t, -1000, rinshan);
    }

    if (who == mSelf && usingCloud(Cloud::WHITE))
        for (int i = 18; i < 27; i++) // bamboo
            mount.lightA(T34(i), 120, rinshan);
}

void Sawaya::onIrsChecked(const Table &table, Mount &mount)
{
    (void) table;

    if (mPredice) {
        for (int i = 0; i < Cloud::NUM_CLOUD; i++) {
            if (mCloudCtrl.itemAt(i).on()) { // consume cloud
                mCloudCtrl.setAbleAt(i, false);
                if (i == Cloud::RED) {
                    mCloudCtrl.setAbleAt(Cloud::RED_SELF, false);
                    mCloudCtrl.setAbleAt(Cloud::RED_RIVALS, false);
                }
            }
        }
    } else {
        for (int i = 0; i < Kamuy::NUM_KAMUY; i++) {
            if (mKamuyCtrl.itemAt(i).on()) { // consume kamuy
                mKamuyCtrl.setAbleAt(i, false);
                if (i == Kamuy::PA_KOR) {
                    mKamuyCtrl.setAbleAt(Kamuy::PA_KOR_R, false);
                    mKamuyCtrl.setAbleAt(Kamuy::PA_KOR_C, false);
                    mKamuyCtrl.setAbleAt(Kamuy::PA_KOR_L, false);
                }
            }
        }

        if (usingKamuy(Kamuy::AT_KOR)) {
            // make dora red
            // too many Superposition does not matter
            for (int pos = 0; pos < 5; pos++) {
                for (T34 t : tiles34::ALL34) {
                    int mk = t.suit() == Suit::M ? 1000 : -50;
                    mount.power(Mount::DORAHYOU, pos, t, mk, false);
                    mount.power(Mount::URAHYOU, pos, t, mk, false);
                }
            }
        }

        if (usingKamuy(Kamuy::PA_KOR)) {
            mConsumedPaKor = true;
            if (mKamuyCtrl.itemAt(Kamuy::PA_KOR_R).on())
                mPaKorTarget = mSelf.right();
            else if (mKamuyCtrl.itemAt(Kamuy::PA_KOR_C).on())
                mPaKorTarget = mSelf.cross();
            else if (mKamuyCtrl.itemAt(Kamuy::PA_KOR_L).on())
                mPaKorTarget = mSelf.left();
        }
    }
}

void Sawaya::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                       std::bitset<Girl::NUM_NM_SKILL> &presence,
                       const Princess &princess)
{
    (void) rand;
    (void) init;
    (void) princess;

    if (usingCloud(Cloud::RED) && usingCloud(Cloud::RED_RIVALS)) {
        // (foolish) prevent z-tiles from being wallable dora
        // to be compatable with kuro and awai
        // one possible smarter sol: use a who-is-here mask as condition
        for (T34 t : tiles34::Z7)
            mount.loadB(T37(t.id34()), 1); // one is enough to make it non-wallable

    }

    if (usingCloud(Cloud::WHITE))
        presence.set(WHITE_CLOUD);
}

bool Sawaya::canUseRedCloud(unsigned &mask) const
{
    if (mPredice && mCloudCtrl.itemAt(Cloud::RED).able()) {
        mask = 0;
        mask |= (1 << Cloud::RED);
        mask |= (1 << Cloud::RED_RIVALS);
        return true;
    } else {
        return false;
    }
}

bool Sawaya::canUseWhiteCloud(unsigned &mask) const
{
    if (mPredice && mCloudCtrl.itemAt(Cloud::WHITE).able()) {
        mask = 0;
        mask |= (1 << Cloud::WHITE);
        return true;
    } else {
        return false;
    }
}

///
/// \deprecated Reduced to 'usingCloud'
///
bool Sawaya::usingRedCloud() const
{
    return usingCloud(Cloud::RED);
}

Girl::IrsCtrlGetter Sawaya::attachIrsOnDice()
{
    bool hasCloud = mCloudCtrl.choices().irsCheck().any();
    return hasCloud ? &Sawaya::mCloudCtrl : nullptr;
}

Girl::IrsCtrlGetter Sawaya::attachIrsOnDrawn(const Table &table)
{
    const Hand &hand = table.getHand(mSelf);
    mKamuyCtrl.setAbleAt(Kamuy::PA_KOR, !mConsumedPaKor && hand.ready());

    if (mKamuyCtrl.checkChoices().irsCheck().any()) {
        mKamuyCtrl.setClickHost(table.getChoices(mSelf));
        return &Sawaya::mKamuyCtrl;
    }

    return nullptr;
}

bool Sawaya::usingCloud(Sawaya::Cloud which) const
{
    return mCloudCtrl.itemAt(which).on();
}

bool Sawaya::usingKamuy(Sawaya::Kamuy which) const
{
    return mKamuyCtrl.itemAt(which).on();
}



} // namespace saki
