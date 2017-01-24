#include "girls_asakumi.h"
#include "table.h"
#include "hand.h"
#include "util.h"

#include <numeric>
#include <sstream>



namespace saki
{



bool Shino::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who != mSelf || iter > 100)
        return true;

    int pairCt = 0;
    for (int ti = 0; ti < 34; ti++) {
        if (init.closed().ct(T34(ti)) >= 3)
            return false; // no triplet
        if (init.closed().ct(T34(ti)) == 2)
            pairCt++;
    }

    return pairCt <= 2;
}

void Shino::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;


    const Hand &hand = table.getHand(mSelf);

    bool south3 = table.getRuleInfo().roundLimit - table.getRound() <= 2
            && table.getRank(mSelf) != 1;
    int mk = south3 ? 1000 : 100;

    if (hand.ready()) {
        using namespace tiles34;
        if (hand.hasEffA(1_s))
            mount.lightA(1_s, 10 * mk);
        if (table.getFuriten(mSelf).any())
            accelerate(mount, hand, table.getRiver(mSelf), 4 * mk);
    } else {
        powerPinfuIipei(hand, table.getRiver(mSelf), mount, mk);
        power3sk(hand, mount, mk);
        powerChanta(hand, mount, mk);
    }
}

void Shino::onDiscarded(const Table &table, Who who)
{
    if (who == mSelf) // self see what see?
        return;

    const Hand &hand = table.getHand(who);
    const T37 &t = table.getFocusTile();

    if (table.riichiEstablished(who) || hand.closed().ct(t) < 1)
        return;

    if (hand.hasEffA(t)) {
        mBreakPair = t;
        table.popUp(mSelf);
    }
}

std::string Shino::popUpStr() const
{
    std::ostringstream oss;
    oss << mBreakPair << "....";
    return oss.str();
}

void Shino::powerPinfuIipei(const Hand &hand, const std::vector<T37> &river,
                            Mount &mount, int mk)
{
    if (!hand.isMenzen())
        return;

    const TileCount &closed = hand.closed();
    int maxIpkSum = 0;
    std::vector<T34> drags;
    for (Suit s : { Suit::M, Suit::P, Suit::S }) {
        for (int v = 1; v <= 7; v++) {
            T34 lt(s, v);
            T34 mt(s, v + 1);
            T34 rt(s, v + 2);
            int l = closed.ct(lt);
            int m = closed.ct(mt);
            int r = closed.ct(rt);

            // fill bad-wait to make higher pinfu rate
            if (v == 1 && r < l && r < m) // left 12 sider
                drags.push_back(rt);
            if (v == 7 && l < m && l < r) // right 89 sider
                drags.push_back(lt);
            if (m < l && m < r) // clamper
                drags.push_back(mt);

            // power-up iipeikou, ignore part more than 2
            l = std::min(l, 2);
            m = std::min(m, 2);
            r = std::min(r, 2);
            int sum = l + m + r;
            if (sum == 6) { // already got a 1pk.
                maxIpkSum = 6;
                drags.clear();
            } else if (sum >= 4 && sum > maxIpkSum) {
                maxIpkSum = sum;
                drags.clear();
                if (l < 2)
                    drags.push_back(lt);
                if (m < 2)
                    drags.push_back(mt);
                if (r < 2)
                    drags.push_back(rt);
            }
        }
    }

    eraseRivered(drags, river);
    for (T34 t : drags)
        mount.lightA(t, mk);
}

void Shino::power3sk(const Hand &hand, Mount &mount, int mk)
{
    if (!hand.isMenzen())
        return;

    const TileCount &closed = hand.closed();
    int maxSum = 0;
    std::vector<T34> powerSsk;
    for (int v = 1; v <= 7; v++) {
        std::array<T34, 9> ts { // a set of sanshoku
            T34(Suit::M, v), T34(Suit::M, v + 1), T34(Suit::M, v + 2),
            T34(Suit::P, v), T34(Suit::P, v + 1), T34(Suit::P, v + 2),
            T34(Suit::S, v), T34(Suit::S, v + 1), T34(Suit::S, v + 2),
        };
        std::array<bool, 9> having;
        std::transform(ts.begin(), ts.end(), having.begin(),
                       [&closed](T34 t){ return closed.ct(t) != 0; });
        int sum = std::accumulate(having.begin(), having.end(), 0);
        if (sum == 9) { // had sanshoku already
            maxSum = 9;
            powerSsk.clear(); // no need to drag any tile
            break;
        } else if (6 <= sum && sum > maxSum) {
            maxSum = sum;
            powerSsk.clear();
            for (int i = 0; i < 9; i++)
                if (!having[i])
                    powerSsk.push_back(ts[i]);
        }
    }

    for (T34 t : powerSsk)
        mount.lightA(t, mk);
}

void Shino::powerChanta(const Hand &hand, Mount &mount, int mk)
{
    if (!hand.isMenzen())
        return;

    const TileCount &closed = hand.closed();
    // id37 of 1m, 7m, 1p, 7p, 1s, 7s
    using namespace tiles34;
    const std::array<T34, 6> heads { 1_m, 7_m, 1_p, 7_p, 1_s, 7_s };
    std::array<int, 6> totalHaving;
    totalHaving.fill(0);
    for (int i = 0; i < 6; i++) {
        std::array<T34, 3> ts { heads[i], heads[i].next(), heads[i].nnext() };
        std::array<int, 3> cts { closed.ct(ts[0]), closed.ct(ts[1]), closed.ct(ts[2]) };
        std::array<bool, 3> having2 { cts[0] >= 2, cts[1] >= 2, cts[2] >= 2 };
        if (having2[0] + having2[1] + having2[2] >= 2) {
            totalHaving[i] = 2;
        } else {
            std::array<bool, 3> having1 { cts[0] >= 0, cts[1] >= 0, cts[2] >= 0 };
            if (having1[0] + having1[1] + having1[2] >= 2)
                totalHaving[i] = 1;
        }
    }

    if (std::accumulate(totalHaving.begin(), totalHaving.end(), 0) >= 3) {
        for (int i = 0; i < 6; i++) {
            if (totalHaving[i] > 0) {
                std::array<T34, 3> ts { heads[i], heads[i].next(), heads[i].nnext() };
                for (int j = 0; j < 3; j++)
                    if (closed.ct(ts[j]) == 0)
                        mount.lightA(ts[j], mk);
            }
        }
    }
}



void Kyouka::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const TileCount &closed = table.getHand(mSelf).closed();

    for (int ti = 0; ti < 34; ti++) {
        T34 t(ti);
        int ct = closed.ct(t);
        const std::array<int, 5> deltas { 0, 40, 140, 0, 0 };
        mount.lightA(t, deltas[ct]);
    }
}



} // namespace saki


