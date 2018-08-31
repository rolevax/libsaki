#include "achiga_ako.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"

#include <cstdlib>



namespace saki
{



void Ako::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(mSelf);
    std::vector<T34> seqHeads;
    for (const M37 &m : hand.barks())
        if (m.type() == M37::Type::CHII)
            seqHeads.push_back(m[0]);

    if (seqHeads.size() == 1) {
        oneDragTwo(mount, hand.closed(), seqHeads[0]);
    } else if (seqHeads.size() == 2) {
        bool sameSuit = seqHeads[0].suit() == seqHeads[1].suit();
        int absValDiff = std::abs(seqHeads[0].val() - seqHeads[1].val());
        bool like3 = !sameSuit && seqHeads[0].val() == seqHeads[1].val();
        bool like1 = sameSuit && (absValDiff == 3 || absValDiff == 6);
        if (like3) {
            int s0 = static_cast<int>(seqHeads[0].suit());
            int s1 = static_cast<int>(seqHeads[1].suit());
            Suit s = static_cast<Suit>(3 - (s0 + s1));
            thinFill(mount, hand.closed(), T34(s, seqHeads[0].val()));
        } else if (like1) {
            int v = 12 - (seqHeads[0].val() + seqHeads[1].val());
            thinFill(mount, hand.closed(), T34(seqHeads[0].suit(), v));
        } else {
            int dist3A = sskDist(hand.closed(), seqHeads[0]);
            int dist1A = ittDist(hand.closed(), seqHeads[0]);
            int dist3B = sskDist(hand.closed(), seqHeads[1]);
            int dist1B = ittDist(hand.closed(), seqHeads[1]);
            int minA = std::min(dist3A, dist1A);
            int minB = std::min(dist3B, dist1B);
            if (minA < minB) {
                oneDragTwo(mount, hand.closed(), seqHeads[0]);
            } else {
                oneDragTwo(mount, hand.closed(), seqHeads[1]);
            }
        }
    }

    const auto &drids = mount.getDrids();
    accelerate(mount, hand, table.getRiver(mSelf), 15);
    if (hand.ctAka5() + drids % hand < 1) {
        for (const T37 &t : drids)
            mount.lightA(t.dora(), 80);

        mount.lightA(T37(Suit::M, 0), 30);
        mount.lightA(T37(Suit::P, 0), 30);
        mount.lightA(T37(Suit::S, 0), 30);
    }
}

int Ako::sskDist(const TileCount &c, T34 head)
{
    assert(head.isNum());
    Suit a = static_cast<Suit>((static_cast<int>(head.suit()) + 1) % 3);
    Suit b = static_cast<Suit>((static_cast<int>(head.suit()) + 2) % 3);
    T34 ah(a, head.val());
    T34 bh(b, head.val());
    int aaa = (c.ct(ah) > 0) + (c.ct(ah.next()) > 0) + (c.ct(ah.nnext()) > 0);
    int bbb = (c.ct(bh) > 0) + (c.ct(bh.next()) > 0) + (c.ct(bh.nnext()) > 0);
    return 6 - (aaa + bbb);
}

int Ako::ittDist(const TileCount &closed, T34 head)
{
    assert(head.isNum());

    Suit s = head.suit();
    int v = head.val();
    int sum = 0;

    if (v == 1) {
        for (int i = 4; i <= 9; i++)
            sum += (closed.ct(T34(s, i)) > 0);
    } else if (v == 4) {
        for (int i = 1; i <= 3; i++)
            sum += (closed.ct(T34(s, i)) > 0);

        for (int i = 7; i <= 9; i++)
            sum += (closed.ct(T34(s, i)) > 0);
    } else if (v == 7) {
        for (int i = 1; i <= 6; i++)
            sum += (closed.ct(T34(s, i)) > 0);
    } else {
        sum = -3;
    }

    return 6 - sum;
}

void Ako::oneDragTwo(Mount &mount, const TileCount &closed, T34 head)
{
    int dist3 = sskDist(closed, head);
    int dist1 = ittDist(closed, head);
    if (dist3 < dist1) {
        if (0 < dist3 && dist3 <= 3) {
            for (Suit s : { Suit::M, Suit::P, Suit::S }) {
                if (s == head.suit())
                    continue;

                thinFill(mount, closed, T34(s, head.val()));
            }
        }
    } else {
        if (0 < dist1 && dist1 <= 3) {
            for (int v : { 1, 4, 7 }) {
                if (v == head.val())
                    continue;

                thinFill(mount, closed, T34(head.suit(), v));
            }
        }
    }
}

void Ako::thinFill(Mount &mount, const TileCount &closed, T34 head)
{
    for (T34 t : { head, head.next(), head.nnext() })
        if (closed.ct(t) == 0)
            mount.lightA(t, 600);
}



} // namespace saki
