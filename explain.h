#ifndef SAKI_EXPLAIN_H
#define SAKI_EXPLAIN_H

#include "tile.h"
#include "hand.h"

#include <array>
#include <vector>



namespace saki
{



///
/// \brief Parsed 4-meld complete hand
///
/// Head division:
/// [ sequences | open 3 | closed 3 | open 4 | closed 4 ]
///
/// 'b' for inclusive begin, 'e' for exclusive end
/// 'x34' for both 3 and 4
/// sequences are sorted by id34-order
/// triplets and quads are not sorted
///
class Explain4
{
public:
    explicit Explain4(const std::vector<T34> &heads, Wait wait, T34 pair,
                      int o3Ct, int c3Ct, int o4Ct, int c4Ct);

    static std::vector<Explain4> make(const TileCount &count, const std::vector<M37> &barks,
                                      T34 pick, bool ron);

    const std::array<T34, 4> &heads() const;
    Wait wait() const;
    T34 pair() const;

    using Iter = std::array<T34, 4>::const_iterator;

    Iter sb() const;
    Iter se() const;
    Iter o3b() const;
    Iter o3e() const;
    Iter c3b() const;
    Iter c3e() const;
    Iter o4b() const;
    Iter o4e() const;
    Iter c4b() const;
    Iter c4e() const;
    Iter x34b() const;
    Iter x34e() const;

    int numS() const;
    int numX34() const;
    int numO3() const;
    int numC3() const;
    int numO4() const;
    int numC4() const;

private:
    static void mapWait(std::vector<Explain4> &res, T34 pick, bool ron, T34 pair,
                        const std::vector<T34> &sHeads,
                        const std::vector<T34> &o3Heads, const std::vector<T34> &c3Heads,
                        const std::vector<T34> &o4Heads, const std::vector<T34> &c4Heads);

private:
    std::array<T34, 4> mHeads;
    Wait mWait;
    T34 mPair;
    int mO3b;
    int mC3b;
    int mO4b;
    int mC4b;
};



} // namespace saki



#endif // SAKI_EXPLAIN_H
