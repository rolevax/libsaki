#ifndef SAKI_UTIL_STACTOR_H
#define SAKI_UTIL_STACTOR_H

#include <array>
#include <functional>
#include <cassert>



namespace saki
{



namespace util
{



template<typename T>
class Range
{
public:
    explicit Range(const T *begin, const T *end) : mBegin(begin) , mEnd(end) {}
    const T *begin() const { return mBegin; }
    const T *end() const { return mEnd; }

private:
    const T *mBegin;
    const T *mEnd;
};

/// Stactor = statically allocated vector
/// similar as boost::static_vector, re-inventing wheels
template<typename T, size_t MAX>
class Stactor
{
public:
    Stactor() = default;
    Stactor(const Stactor &copy) = default;
    Stactor &operator=(const Stactor &assign) = default;
    ~Stactor() = default;

    Stactor(std::initializer_list<T> inits)
    {
        for (const auto &e : inits)
            pushBack(e);
    }

	using ArrayType = std::array<T, MAX>;

    using Iterator = typename ArrayType::iterator;
    using ConstIterator = typename ArrayType::const_iterator;

    /// enable std funcs
    using value_type = T;

    T &operator[](size_t i)
	{
		assert(i < mSize);
		return mArray[i];
	}

    const T &operator[](size_t i) const
	{
		assert(i < mSize);
		return mArray[i];
	}

    T &at(size_t i)
    {
        return (*this)[i];
    }

    const T &at(size_t i) const
    {
        return (*this)[i];
    }

    bool empty() const noexcept
    {
        return mSize == 0;
    }

    size_t size() const noexcept
    {
        return mSize;
    }

    Iterator begin() noexcept
	{
		return mArray.begin();
	}

    ConstIterator begin() const noexcept
    {
        return mArray.begin();
    }

    Iterator end() noexcept
	{
		return mArray.begin() + mSize;
	}

    ConstIterator end() const noexcept
    {
        return mArray.begin() + mSize;
    }

    Range<T> range() const noexcept
    {
        return Range<T>(mArray.data(), mArray.data() + mSize);
    }

    T &front()
    {
        assert(!empty());
        return operator[](0);
    }

    const T &front() const
    {
        assert(!empty());
        return operator[](0);
    }

    T &back()
    {
        assert(!empty());
        return operator[](mSize - 1);
    }

    const T &back() const
    {
        assert(!empty());
        return operator[](mSize - 1);
    }

    void pushBack(const T &elem)
    {
        assert(mSize + 1 <= MAX);
        mArray[mSize++] = elem;
    }

    void pushBack(T &&elem)
    {
        assert(mSize + 1 <= MAX);
        mArray[mSize++] = elem;
    }

    void pushBack(const Range<T> &range)
    {
        for (const T &v : range)
            pushBack(v);
    }

    /// alias to enable std::back_inserter
    void push_back(const T &elem)
    {
        pushBack(elem);
    }

    /// alias to enable std::back_inserter
    void push_back(T &&elem)
    {
        pushBack(elem);
    }

    void popBack()
    {
        assert(!empty());
        mSize--;
    }

    void clear()
    {
        mSize = 0;
    }

    void exile(size_t i)
    {
        assert(i < mSize);
        mArray[i] = back();
        mSize--;
    }

    void exileAllIf(std::function<bool(const T &)> pred)
    {
        for (size_t i = 0; i < mSize; i++)
            if (pred(mArray[i]))
                exile(i--); // stay
    }

    Stactor<T, MAX> maxs(std::function<int(const T&)> measure, int floor) const
    {
        int max = floor;
        Stactor<T, MAX> res;

        for (const auto &a : *this) {
            int comax = measure(a);

            if (comax > max) {
                max = comax;
                res.clear();
            }

            if (comax == max)
                res.pushBack(a);
        }

        return res;
    }

private:
	ArrayType mArray;
    size_t mSize = 0;
};



} // namespace util



} // namespace saki




#endif // SAKI_UTIL_STACTOR_H


