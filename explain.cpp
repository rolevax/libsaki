#include "explain.h"
#include "meld.h"
#include "tile.h"
#include "hand.h"

#include <algorithm>



namespace saki
{



Explain4::Explain4(const std::vector<T34> &heads, Wait wait, T34 pair,
                   int o3Ct, int c3Ct, int o4Ct, int c4Ct)
    : mHeads { heads[0], heads[1], heads[2], heads[3] }
    , mWait(wait)
    , mPair(pair)
    , mO3b(4 - (o3Ct + c3Ct + o4Ct + c4Ct))
    , mC3b(mO3b + o3Ct)
    , mO4b(mC3b + c3Ct)
    , mC4b(mO4b + o4Ct)
{
    assert(heads.size() == 4);
    assert(0 <= mO3b && mO3b <= mC3b && mC3b <= mO4b && mO4b <= mC4b && mC4b <= 4);
}

std::vector<Explain4> Explain4::make(const TileCount &count, const std::vector<M37> &barks,
                                     T34 pick, bool ron)
{
    std::vector<Explain4> res;

    std::vector<TileCount::Explain4Closed> expCloseds = count.explain4(pick);

    for (TileCount::Explain4Closed &exp : expCloseds) {
        assert(exp.sequences.size() + exp.triplets.size() + barks.size() == 4);

        auto insertSequence = [&exp](T34 head) {
            auto &tar = exp.sequences;
            auto it = std::find_if_not(tar.begin(), tar.end(),
                                       [head](T34 t) { return head < t; });
            tar.insert(it, head);
        };

        std::vector<T34> o3Heads;
        std::vector<T34> o4Heads;
        std::vector<T34> c4Heads;

        // include barks
        for (const M37 &m : barks) {
            switch (m.type()) {
            case M37::Type::CHII:
                insertSequence(m[0]); // keep sequences ordered
                break;
            case M37::Type::PON:
                o3Heads.push_back(m[0]);
                break;
            case M37::Type::DAIMINKAN:
            case M37::Type::KAKAN:
                o4Heads.push_back(m[0]);
                break;
            case M37::Type::ANKAN:
                c4Heads.push_back(m[0]);
                break;
            }
        }

        mapWait(res, pick, ron, exp.pair,
                exp.sequences, o3Heads, exp.triplets, o4Heads, c4Heads);
    }

    assert(!res.empty());
    return res;
}

const std::array<T34, 4> &Explain4::heads() const
{
    return mHeads;
}

Wait Explain4::wait() const
{
    return mWait;
}

T34 Explain4::pair() const
{
    return mPair;
}

Explain4::Iter Explain4::sb() const
{
    return mHeads.begin();
}

Explain4::Iter Explain4::se() const
{
    return sb() + mO3b;
}

Explain4::Iter Explain4::o3b() const
{
    return sb() + mO3b;
}

Explain4::Iter Explain4::o3e() const
{
    return sb() + mC3b;
}

Explain4::Iter Explain4::c3b() const
{
    return sb() + mC3b;
}

Explain4::Iter Explain4::c3e() const
{
    return sb() + mO4b;
}

Explain4::Iter Explain4::o4b() const
{
    return sb() + mO4b;
}

Explain4::Iter Explain4::o4e() const
{
    return sb() + mC4b;
}

Explain4::Iter Explain4::c4b() const
{
    return sb() + mC4b;
}

Explain4::Iter Explain4::c4e() const
{
    return mHeads.end();
}

Explain4::Iter Explain4::x34b() const
{
    return o3b();
}

Explain4::Iter Explain4::x34e() const
{
    return c4e();
}

int Explain4::numS() const
{
    return se() - sb();
}

int Explain4::numX34() const
{
    return 4 - numS();
}

int Explain4::numO3() const
{
    return o3e() - o3b();
}

int Explain4::numC3() const
{
    return c3e() - c3b();
}

int Explain4::numO4() const
{
    return o4e() - o4b();
}

int Explain4::numC4() const
{
    return c4e() - c4b();
}

void Explain4::mapWait(std::vector<Explain4> &res, T34 pick, bool ron, T34 pair,
                       const std::vector<T34> &sHeads,
                       const std::vector<T34> &o3Heads,
                       const std::vector<T34> &c3Heads,
                       const std::vector<T34> &o4Heads,
                       const std::vector<T34> &c4Heads)
{
    if (pick == pair) {
        std::vector<T34> heads(sHeads); // copy
        heads.insert(heads.end(), o3Heads.begin(), o3Heads.end());
        heads.insert(heads.end(), c3Heads.begin(), c3Heads.end());
        heads.insert(heads.end(), o4Heads.begin(), o4Heads.end());
        heads.insert(heads.end(), c4Heads.begin(), c4Heads.end());
        res.emplace_back(heads, Wait::ISORIDE, pair,
                         o3Heads.size(), c3Heads.size(),
                         o4Heads.size(), c4Heads.size());
    }

    for (size_t i = 0; i < c3Heads.size(); i++) {
        if (c3Heads[i] == pick) {
            /// create bi-bump, mind open/closed by 'ron'
            std::vector<T34> heads(sHeads); // copy
            heads.insert(heads.end(), o3Heads.begin(), o3Heads.end());
            if (ron) {
                heads.push_back(c3Heads[i]); // one more open-3
                // filter-out one closed-3
                for (size_t j = 0; j < c3Heads.size(); j++)
                    if (i != j)
                        heads.push_back(c3Heads[j]);
            } else {
                heads.insert(heads.end(), c3Heads.begin(), c3Heads.end());
            }
            heads.insert(heads.end(), o4Heads.begin(), o4Heads.end());
            heads.insert(heads.end(), c4Heads.begin(), c4Heads.end());
            res.emplace_back(heads, Wait::BIBUMP, pair,
                             o3Heads.size() + ron, c3Heads.size() - ron,
                             o4Heads.size(), c4Heads.size());
        }
    }

    for (size_t i = 0; i < sHeads.size(); i++) {
        Wait wait = sHeads[i].waitAsSequence(pick);
        if (wait != Wait::NONE) {
            std::vector<T34> heads(sHeads); // copy
            heads.insert(heads.end(), o3Heads.begin(), o3Heads.end());
            heads.insert(heads.end(), c3Heads.begin(), c3Heads.end());
            heads.insert(heads.end(), o4Heads.begin(), o4Heads.end());
            heads.insert(heads.end(), c4Heads.begin(), c4Heads.end());
            res.emplace_back(heads, wait, pair,
                             o3Heads.size(), c3Heads.size(),
                             o4Heads.size(), c4Heads.size());
        }
    }
}



} // namespace saki


