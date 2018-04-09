#include "x_yue.h"
#include "../table/table.h"
#include "../util/misc.h"

#include <numeric>
#include <sstream>



namespace saki
{



void Yue::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) rinshan;

    if (who != mSelf)
        return;

    using namespace tiles34;
    std::vector<T34> guests;
    std::vector<T34> hosts;
    {
        int sw = table.getSelfWind(mSelf);
        int rw = table.getRoundWind();
        guests.reserve(3);
        hosts.reserve(2);
        for (T34 t : { 1_f, 2_f, 3_f, 4_f })
            (t.isYakuhai(sw, rw) ? hosts : guests).push_back(t);
    }

    const Hand &hand = table.getHand(mSelf);
    int step4 = hand.step4();

    T34 maxGuest;
    int maxGuestCt = 0;
    for (T34 g : guests) {
        int comax = countGuest(hand, g);
        if (comax > maxGuestCt) {
            maxGuest = g;
            maxGuestCt = comax;
        }
    }

    if (step4 <= 1) {
        if (hand.isMenzen()) { // ignore barked cases
            if (!dyed(hand)) {
                for (T34 t : hand.effA())
                    mount.lightA(t, -50);
            } else {
                if (maxGuestCt == 2) {
                    mount.lightA(maxGuest, 300);
                } else if (maxGuestCt >= 3) {
                    for (T34 t : hand.effA())
                        mount.lightA(t, 250);
                }
            }
        }
    } else {
        if (maxGuestCt == 0) {
            for (T34 t : guests)
                mount.lightA(t, 500);
        } else if (1 <= maxGuestCt && maxGuestCt <= 2) {
            mount.lightA(maxGuest, 500);
        }

        if (maxGuestCt == 2)
            dye(hand.closed(), mount, 100);
        else if (maxGuestCt >= 3)
            dye(hand.closed(), mount, 200);

        for (T34 h : hosts) // reject closed host triplet
            if (hand.closed().ct(h) >= 2)
                mount.lightA(h, -50);
    }
}

void Yue::dye(const TileCount &closed, Mount &mount, int mk)
{
    std::array<int, 3> ofss { 0, 9, 18 };
    // *INDENT-OFF*
    auto sum = [&closed](int ofs) {
        int s = 0;
        for (int i = 0; i < 9; i++)
            s += closed.ct(T34(ofs + i));

        return s;
    };
    // *INDENT-ON*
    std::array<int, 3> sums;
    std::transform(ofss.begin(), ofss.end(), sums.begin(), sum);
    int ofs = ofss[std::max_element(sums.begin(), sums.end()) - sums.begin()];
    for (int i = 0; i < 9; i++)
        mount.lightA(T34(ofs + i), mk);
}

bool Yue::dyed(const Hand &hand)
{
    int suitCt = 0;

    for (Suit s : { Suit::M, Suit::P, Suit::S }) {
        for (int v = 1; v <= 9; v++) {
            if (hand.ct(T34(s, v)) > 0) {
                suitCt++;
                if (suitCt >= 2)
                    return false;

                break; // go for next suit
            }
        }
    }

    return true;
}

int Yue::countGuest(const Hand &hand, T34 g)
{
    // count 'closed' only
    // *INDENT-OFF*
    auto isAnkan = [g](const M37 &m) {
        return m.type() == M37::Type::ANKAN && m[0] == g;
    };
    // *INDENT-ON*

    if (util::any(hand.barks(), isAnkan))
        return 4;

    return hand.closed().ct(g);
}



} // namespace saki
