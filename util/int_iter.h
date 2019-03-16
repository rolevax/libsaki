#ifndef SAKI_INT_ITER_H
#define SAKI_INT_ITER_H

#include <iterator>



namespace saki
{



template<typename T>
class IntIter
{
public:
    constexpr explicit IntIter(int c) : mCurr(c) {}

    // std input iterator traits
    using iterator_category = std::input_iterator_tag;
    using difference_type = int;
    using value_type = T;
    using pointer = T *;
    using reference = T &;

    T operator*()
    {
        return T(mCurr);
    }

    IntIter &operator++() noexcept
    {
        ++mCurr;
        return *this;
    }

    bool operator==(IntIter that) const noexcept
    {
        return mCurr == that.mCurr;
    }

    bool operator!=(IntIter that) const noexcept
    {
        return !(*this == that);
    }

private:
    int mCurr;
};



template<typename T, int END>
class IntRange
{
public:
    using iterator = IntIter<T>;
    using value_type = T;

    constexpr IntIter<T> begin() const
    {
        return IntIter<T>(0);
    }

    constexpr IntIter<T> end() const
    {
        return IntIter<T>(END);
    }
};



} // namespace saki



#endif // SAKI_INT_ITER_H
