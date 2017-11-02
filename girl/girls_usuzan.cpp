#include "girls_usuzan.h"
#include "../table/table.h"
#include "../util/misc.h"



namespace saki
{



void Sawaya::onDice(util::Rand &rand, const Table &table, Choices &choices)
{
    (void) rand;
    (void) table;

    mPredice = true;
    mPaKorTarget = Who();

    for (IrsCheckRow &r : mClouds)
        if (!r.mono && !r.able)
            r.on = false;

    for (int i = 0; i < Kamuy::NUM_KAMUY; i++) {
        if (!mKamuys[i].mono && !mKamuys[i].able) {
            if (i == Kamuy::HURI) {
                if (!mConsumedSecondHuri) {
                    mConsumedSecondHuri = true;
                    mKamuys[i].on = true;
                } else {
                    mKamuys[i].on = false;
                }
            } else {
                mKamuys[i].on = false; // TODO off hoyaw-kamuy by resist-time
            }
        }
    }

    if (util::any(mClouds, [](const IrsCheckRow &r) { return r.able; })) {
        mChoicesBackup = choices;
        choices.setCut();
    }
}

void Sawaya::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;

    mPredice = false;

    if (mClouds[Cloud::RED].on && mClouds[Cloud::RED_RIVALS].on) {
        for (T34 t : tiles34::Z7)
            for (int w = 0; w < 4; w++)
                exists[w].inc(t, w == mSelf.index() ? 100 : -1000);
    } else if (mClouds[Cloud::RED].on && mClouds[Cloud::RED_SELF].on) {
        for (T34 t : tiles34::Z7)
            exists[mSelf.index()].inc(t, -1000);
    }

    if (mClouds[Cloud::WHITE].on)
        for (int i = 18; i < 27; i++) // bamboo
            exists[mSelf.index()].inc(T34(i), 70);
}

void Sawaya::onActivate(const Table &table, Choices &choices)
{
    const Hand &hand = table.getHand(mSelf);
    mKamuys[Kamuy::PA_KOR].able = !mConsumedPaKor && hand.ready();

    if (util::none(mKamuys, [](const IrsCheckRow &r) { return r.able; }))
        return;

    if (choices.can(ActCode::SWAP_OUT) || choices.can(ActCode::SPIN_OUT)) {
        mChoicesBackup = choices;
        choices.setExtra(true);
    }
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

    if (mKamuys[Kamuy::AT_KOR].on) {
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

    if (who == mSelf && mKamuys[Kamuy::HURI].on) {
        for (int i = 1; i <= 4; i++) {
            if (i == table.getSelfWind(mSelf))
                continue;

            T34 t(Suit::F, i);
            const TileCount &closed = table.getHand(mSelf).closed();
            if (closed.ct(t) == 1)
                mount.lightA(t, 1000, rinshan);
        }
    }

    if (mClouds[Cloud::RED].on && mClouds[Cloud::RED_RIVALS].on) {
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
    } else if (mClouds[Cloud::RED].on && mClouds[Cloud::RED_SELF].on) {
        if (who == mSelf) {
            for (T34 t : tiles34::Z7)
                mount.lightA(t, -1000, rinshan);
        }
    }

    if (who == mSelf && mClouds[Cloud::WHITE].on)
        for (int i = 18; i < 27; i++) // bamboo
            mount.lightA(T34(i), 120, rinshan);
}

const IrsCheckRow &Sawaya::irsCheckRow(int index) const
{
    if (mPredice) {
        assert(0 <= index && index < Cloud::NUM_CLOUD);
        return mClouds[index];
    } else {
        assert(0 <= index && index < Kamuy::NUM_KAMUY);
        return mKamuys[index];
    }
}

int Sawaya::irsCheckCount() const
{
    return mPredice ? static_cast<int>(Cloud::NUM_CLOUD)
                    : static_cast<int>(Kamuy::NUM_KAMUY);
}

Choices Sawaya::forwardAction(const Table &table, Mount &mount, const Action &action)
{
    assert(action.isIrs());

    Choices cut;
    cut.setCut();

    switch (action.act()) {
    case ActCode::IRS_CLICK:
        return cut;
    case ActCode::IRS_CHECK:
        return handleIrsCheck(table, mount, action.mask());
    default:
        unreached("Sawaya::forwardAction");
        break;
    }
}

void Sawaya::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                       std::bitset<Girl::NUM_NM_SKILL> &presence,
                       const Princess &princess)
{
    (void) rand;
    (void) init;
    (void) princess;

    if (mClouds[Cloud::RED].on && mClouds[Cloud::RED_RIVALS].on) {
        // (foolish) prevent z-tiles from being wallable dora
        // to be compatable with kuro and awai
        // one possible smarter sol: use a who-is-here mask as condition
        for (T34 t : tiles34::Z7)
            mount.loadB(T37(t.id34()), 1); // one is enough to make it non-wallable
    }

    if (mClouds[Cloud::WHITE].on)
        presence.set(WHITE_CLOUD);
}

bool Sawaya::canUseRedCloud(unsigned &mask) const
{
    if (mPredice && mClouds[Cloud::RED].able) {
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
    if (mPredice && mClouds[Cloud::WHITE].able) {
        mask = 0;
        mask |= (1 << Cloud::WHITE);
        return true;
    } else {
        return false;
    }
}

bool Sawaya::usingRedCloud() const
{
    return mClouds[Cloud::RED].on;
}

Choices Sawaya::handleIrsCheck(const Table &table, Mount &mount, unsigned mask)
{
    (void) table;

    if (mPredice) {
        for (int i = 0; i < Cloud::NUM_CLOUD; i++) {
            mClouds[i].on = mask & (0b1 << (Cloud::NUM_CLOUD - 1 - i));
            if (mClouds[i].on) { // consume cloud
                mClouds[i].able = false;
                if (i == Cloud::RED) {
                    mClouds[Cloud::RED_SELF].able = false;
                    mClouds[Cloud::RED_RIVALS].able = false;
                }
            }
        }
    } else {
        for (int i = 0; i < Kamuy::NUM_KAMUY; i++) {
            mKamuys[i].on = mask & (0b1 << (Kamuy::NUM_KAMUY - 1 - i));
            if (mKamuys[i].on) { // consume kamuy
                mKamuys[i].able = false;
                if (i == Kamuy::PA_KOR) {
                    mKamuys[Kamuy::PA_KOR_R].able = false;
                    mKamuys[Kamuy::PA_KOR_C].able = false;
                    mKamuys[Kamuy::PA_KOR_L].able = false;
                }
            }
        }

        if (mKamuys[Kamuy::AT_KOR].on) {
            // make dora red
            // too many Superposition does not matter
            for (int pos = 0; pos < 5; pos++) {
                for (T34 t : tiles34::ALL34) {
                    int mk = t.suit() == Suit::M ? 1000 : -50;
                    mount.power(Mount::DORA, pos, t, mk, false);
                    mount.power(Mount::URADORA, pos, t, mk, false);
                }
            }
        }

        if (mKamuys[Kamuy::PA_KOR].on) {
            mConsumedPaKor = true;
            if (mKamuys[Kamuy::PA_KOR_R].on)
                mPaKorTarget = mSelf.right();
            else if (mKamuys[Kamuy::PA_KOR_C].on)
                mPaKorTarget = mSelf.cross();
            else if (mKamuys[Kamuy::PA_KOR_L].on)
                mPaKorTarget = mSelf.left();
        }
    }

    return mChoicesBackup;
}



} // namespace saki


