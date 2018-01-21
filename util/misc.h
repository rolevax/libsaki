#ifndef SAKI_MISC_H
#define SAKI_MISC_H

#include <functional>
#include <algorithm>
#include <iostream>



namespace saki
{



namespace util
{



///
/// @brief same as boost:all_of
///
template<typename V, typename Pred>
inline bool all(const V &v, Pred f)
{
    return std::all_of(v.begin(), v.end(), f);
}

///
/// @brief same as std::all_of
///
template<typename Iter, typename Pred>
inline bool all(Iter begin, Iter end, Pred f)
{
    return std::all_of(begin, end, f);
}

///
/// @brief same as boost::any_of
///
template<typename V, typename Pred>
inline bool any(const V &v, Pred f)
{
    return std::any_of(v.begin(), v.end(), f);
}

///
/// @brief same as std::any_of
///
template<typename Iter, typename Pred>
inline bool any(Iter begin, Iter end, Pred f)
{
    return std::any_of(begin, end, f);
}

///
/// @brief same as boost::none_of
///
template<typename V, typename Pred>
inline bool none(const V &v, Pred f)
{
    return std::none_of(v.begin(), v.end(), f);
}

///
/// @brief same as std::none_of
///
template<typename Iter, typename Pred>
inline bool none(Iter begin, Iter end, Pred f)
{
    return std::none_of(begin, end, f);
}

///
/// @brief same as boost::any_of_equal
///
template<typename V, typename T>
inline bool has(const V &v, T e)
{
    return std::find(v.begin(), v.end(), e) != v.end();
}

///
/// @brief same as boost::any_of_equal
///
template<typename Iter, typename T>
inline bool has(Iter begin, Iter end, T e)
{
    return std::find(begin, end, e) != end;
}

///
/// \brief intersection not empty
///
template<typename V>
inline bool common(V v1, V v2)
{
    auto f = [&v2](typename V::value_type a) { return has(v2, a); };
    return any(v1, f);
}

template<typename V, typename F>
inline V maxs(V inputs, F measure, int floor)
{
    int max = floor;
    V res;

    for (const auto &a : inputs) {
        int comax = measure(a);

        if (comax > max) {
            max = comax;
            res.clear();
        }

        if (comax == max) {
            res.emplace_back(a);
        }
    }

    return res;
}

///
/// @brief alias for 'std::cout << arg << std::endl;', just save typing
///
template<typename T>
inline void p(T arg)
{
    std::cout << arg << std::endl;
}

///
/// @brief alias for 'std::cout << arg1 << arg2 << ... << std::endl;', just save typing
///
template<typename T, typename... Args>
inline void p(T t, Args... args)
{
    std::cout << t << ' ';

    p(args...);
}



} // namespace util



} // namespace saki



#endif // SAKI_UTIL_H
