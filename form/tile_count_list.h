#ifndef SAKI_TILE_COUNT_LIST_H
#define SAKI_TILE_COUNT_LIST_H

#include "tile_count.h"



namespace saki
{



///
/// \brief An iterable virtual list of all tile-counts whose
///        containing tiles sum up to a given number
///
class TileCountList
{
public:
    struct End {};

    class Iter
    {
    public:
        explicit Iter(int sum, T34 min, T34 max);

        // std input iterator traits
        using iterator_category = std::input_iterator_tag;
        using difference_type = int;
        using value_type = const TileCount &;
        using pointer = const TileCount *;
        using reference = const TileCount &;

        const TileCount &operator*() noexcept;
        Iter &operator++() noexcept;
        bool operator==(End that) const noexcept;
        bool operator!=(End that) const noexcept;

    private:
        bool isMax(int beginIndex, int task) const;
        void setMin(int begin, int task);

    private:
        int mSum;
        T34 mMinTile;
        T34 mMaxTile;
        TileCount mCount;
        bool mEnd = false;
    };

    explicit TileCountList(int sum, T34 min = T34(0), T34 max = T34(33));

    Iter begin();
    End end();

private:
    int mSum;
    T34 mMinTile;
    T34 mMaxTile;
};



} // namespace saki



#endif // SAKI_TILE_COUNT_LIST_H
