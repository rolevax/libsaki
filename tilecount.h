#ifndef SAKI_TILECOUNT_H
#define SAKI_TILECOUNT_H

#include "tile.h"

#include <initializer_list>



namespace saki
{



class TileCount
{
public:
    enum AkadoraCount { AKADORA0, AKADORA3, AKADORA4 };

    struct Explain4Closed
    {
        explicit Explain4Closed(T34 p) : pair(p) { }
        T34 pair;
        std::vector<T34> triplets;
        std::vector<T34> sequences;
    };

    TileCount();
    TileCount(AkadoraCount fillMode);
    TileCount(std::initializer_list<T37> t37s);
    TileCount(std::vector<T37> t37s);

    TileCount(const TileCount &copy) = default;
    TileCount &operator=(const TileCount &assign) = default;

    int ct(T34 t) const;
    int ct(const T37 &t) const;
    int ct(const std::vector<T34> &ts) const;
    int ct(const std::vector<T37> &ts) const;
    int ctAka5() const;
    int ctZ() const;
    int ctYao() const;

    bool hasZ() const;
    bool hasYao() const;

    void inc(const T37 &t, int delta);

    TileCount &operator-=(const TileCount &rhs);

    int step(int barkCt) const;
    int stepGb(int barkCt) const;
    int step4(int barkCt) const;
    int step7() const;
    int step7Gb() const;
    int step13() const;

    std::vector<T34> t34s() const;
    std::vector<T37> t37s(bool allowDup = false) const;

    bool dislike4(T34 t) const;

    std::vector<Explain4Closed> explain4(T34 pick) const;
    bool onlyInTriplet(T34 pick, int barkCt) const;

    int sum(const std::vector<T34> &ts) const;
    int sum() const;

private:
    ///
    /// \brief cut-out meld and submeld from the count and get the max work-delta
    /// \param i beginning tild id37
    /// \param maxCut max number of meld to cut
    /// \return step-4 delta
    ///
    int cutMeld(int i, int maxCut) const;

    ///
    /// \brief cut-out submeld from the count and get the max work-delta
    /// \param i beginning tild id37
    /// \param maxCut max number of submeld to cut
    /// \return step-4 delta
    ///
    int cutSubmeld(int i, int maxCut) const;

    bool decomposeBirdless4(Explain4Closed &exp, const std::array<int, 34> &c) const;

private:
    // marked 'mutable' since it will be changed back
    // to the original value after any const computation.
    // be careful within the implementation of this class
    mutable std::array<int, 34> c;
    std::array<int, 3> mAka5s;
};



} // namespace saki



#endif // SAKI_TILECOUNT_H


