#ifndef SAKI_INT_ITER_H
#define SAKI_INT_ITER_H



namespace saki
{



template<typename T>
class IntIter
{
public:
    constexpr explicit IntIter(int c) : mCurr(c) {}
    IntIter(const IntIter &copy) = default;
    IntIter &operator=(const IntIter &assign) = default;

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
