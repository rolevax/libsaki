#include "tile_count_list.h"



namespace saki
{



///
/// \brief Contruct a virtual list of hands formed by tiles in range [min, max]
/// \param sum Sum of tiles in the hand
///
/// By "hand" we mean maximun of each kind of tile is 4
///
TileCountList::TileCountList(int sum, T34 min, T34 max)
    : mSum(sum)
    , mMinTile(min)
    , mMaxTile(max)
{
    assert(0 <= sum && sum <= (max.id34() - min.id34() + 1) * 4);
}

TileCountList::Iter TileCountList::begin()
{
    return Iter(mSum, mMinTile, mMaxTile);
}

TileCountList::End TileCountList::end()
{
    return {};
}

TileCountList::Iter::Iter(int sum, T34 min, T34 max)
    : mSum(sum)
    , mMinTile(min)
    , mMaxTile(max)
{
    assert(0 <= sum && sum <= (max.id34() - min.id34() + 1) * 4);

    setMin(min.id34(), sum);
    mEnd = isMax(min.id34(), mSum);
}

const TileCount &TileCountList::Iter::operator*() noexcept
{
    return mCount;
}

TileCountList::Iter &TileCountList::Iter::operator++() noexcept
{
    if (mEnd)
        return *this;

    int beginOfMax = mMinTile.id34();
    int workOfMax = mSum;

    while (!isMax(beginOfMax, workOfMax)) {
        workOfMax -= mCount.ct(T34(beginOfMax));
        beginOfMax++;
    }

    if (beginOfMax == mMinTile.id34()) {
        mEnd = true;
    } else {
        T37 prev(beginOfMax - 1);
        mCount.inc(prev, 1);
        setMin(beginOfMax, workOfMax - 1);
    }

    return *this;
}

bool TileCountList::Iter::operator==(TileCountList::End that) const noexcept
{
    (void) that;
    return mEnd;
}

bool TileCountList::Iter::operator!=(TileCountList::End that) const noexcept
{
    return !(*this == that);
}

bool TileCountList::Iter::isMax(int beginIndex, int task) const
{
    T34 begin(beginIndex);
    if (beginIndex == mMaxTile.id34() || task < 4)
        return mCount.ct(begin) == task;

    return mCount.ct(begin) == 4 && isMax(beginIndex + 1, task - 4);
}

void TileCountList::Iter::setMin(int begin, int task)
{
    for (int ti = begin; ti <= mMaxTile.id34(); ti++)
        mCount.clear(T34(ti));

    int ti = mMaxTile.id34();
    while (task >= 4) {
        mCount.inc(T37(ti), 4);
        ti--;
        task -= 4;
    }

    mCount.inc(T37(ti), task);
}



} // namespace saki
