#ifndef SAKI_UTIL_STACTOR_H
#define SAKI_UTIL_STACTOR_H

#include <type_traits>
#include <functional>
#include <new>
#include <cassert>



namespace saki
{



namespace util
{



template <class T>
constexpr T* launder(T* p) noexcept
{
#ifdef __clang__
    return p; // clang++ in NDK don't have launder??
#else
    return std::launder(p);
#endif
}



template<typename T>
class Range
{
public:
    explicit Range(const T *begin, const T *end) : mBegin(begin), mEnd(end) {}
    const T *begin() const noexcept { return mBegin; }
    const T *end() const noexcept { return mEnd; }
    bool empty() const noexcept { return mBegin == mEnd; }

private:
    const T *mBegin;
    const T *mEnd;
};



template<typename T, size_t MAX>
class StactorBase
{
public:
    StactorBase() = default;
    StactorBase(const StactorBase &copy) = default;
    StactorBase &operator=(const StactorBase &assign) = default;
    ~StactorBase() = default;

    StactorBase(std::initializer_list<T> inits) noexcept
    {
        for (const auto &e : inits)
            pushBack(e);
    }

    /// enable std funcs
    using value_type = T;
    using iterator = T *;
    using const_iterator = const T *;

    const T *data() const noexcept
    {
        return launder(reinterpret_cast<const T *>(mData));
    }

    T *data() noexcept
    {
        return launder(reinterpret_cast<T *>(mData));
    }

    T &operator[](size_t i) noexcept
    {
        assert(i < mSize);
        return data()[i];
    }

    const T &operator[](size_t i) const noexcept
    {
        assert(i < mSize);
        return data()[i];
    }

    T &at(size_t i) noexcept
    {
        return (*this)[i];
    }

    const T &at(size_t i) const noexcept
    {
        return (*this)[i];
    }

    bool empty() const noexcept
    {
        return mSize == 0;
    }

    bool full() const noexcept
    {
        return mSize == MAX;
    }

    size_t size() const noexcept
    {
        return mSize;
    }

    int iSize() const noexcept
    {
        return static_cast<int>(size());
    }

    T *begin() noexcept
    {
        return data();
    }

    const T *begin() const noexcept
    {
        return data();
    }

    T *end() noexcept
    {
        return begin() + mSize;
    }

    const T *end() const noexcept
    {
        return begin() + mSize;
    }

    Range<T> range() const noexcept
    {
        return Range<T>(begin(), end());
    }

    T &front() noexcept
    {
        assert(!empty());
        return operator[](0);
    }

    const T &front() const noexcept
    {
        assert(!empty());
        return operator[](0);
    }

    T &back() noexcept
    {
        assert(!empty());
        return operator[](mSize - 1);
    }

    const T &back() const noexcept
    {
        assert(!empty());
        return operator[](mSize - 1);
    }

    template<typename... Args>
    void emplaceBack(Args && ... elem) noexcept
    {
        assert(mSize + 1 <= MAX);
        new (mData + mSize) T(std::forward<Args>(elem) ...);
        mSize++;
    }

    void pushBack(const T &elem) noexcept
    {
        assert(mSize + 1 <= MAX);
        new (mData + mSize) T(elem);
        mSize++;
    }

    void pushBack(T &&elem) noexcept
    {
        assert(mSize + 1 <= MAX);
        new (mData + mSize) T(std::move(elem));
        mSize++;
    }

    void pushBack(const Range<T> &range) noexcept
    {
        for (const T &v : range)
            pushBack(v);
    }

    /// alias to enable std::back_inserter
    void push_back(const T &elem) noexcept
    {
        pushBack(elem);
    }

    /// alias to enable std::back_inserter
    void push_back(T &&elem) noexcept
    {
        pushBack(std::forward<T>(elem));
    }

    void popBack() noexcept
    {
        assert(!empty());
        (data() + mSize - 1)->T::~T();
        mSize--;
    }

    void clear() noexcept
    {
        while (mSize > 0)
            popBack();
    }

protected:
    static const size_t ALIGN = std::alignment_of<T>::value;
    typename std::aligned_storage<sizeof(T), ALIGN>::type mData[MAX];
    size_t mSize = 0;
};



///
/// \brief Stactor = statically allocated vector
///
/// Similar to boost::static_vector, re-inventing wheels
///
/// Strength compared to std::vector<T> or std::vector<T, StaticAllocator<T>>:
/// - No dynamic allocation or resizing
/// - No buffer pointer cost
/// - Trivially copyable if T is trivially copyable
///
/// Strength compared to std::array<T, MAX>:
/// - Variable size
/// - No need to fill all elements (by default ctor or explicit init)
///
/// Style difference between STL containers:
/// - Using assertion instead of exception
/// - Naming by camel cases
///
template<typename T, size_t MAX, typename SFINAE = void>
class Stactor;



template<typename T, size_t MAX>
class Stactor<T, MAX, typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
    : public StactorBase<T, MAX>
{
public:
    using StactorBase<T, MAX>::StactorBase;
    ~Stactor() = default;
};



template<typename T, size_t MAX>
class Stactor<T, MAX, typename std::enable_if<!std::is_trivially_copyable<T>::value>::type>
    : public StactorBase<T, MAX>
{
public:
    using Base = StactorBase<T, MAX>;

    using Base::StactorBase;

    ~Stactor()
    {
        Base::clear();
    }

    Stactor(const Stactor &copy)
    {
        for (const auto &e : copy)
            pushBack(e);
    }

    Stactor &operator=(const Stactor &that)
    {
        Base::clear();
        for (const auto &e : that)
            pushBack(e);
    }
};



} // namespace util



} // namespace saki




#endif // SAKI_UTIL_STACTOR_H
