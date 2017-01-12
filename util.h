#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>

namespace saki
{



namespace util
{



///
/// @brief Enable type conversion when expanding templates
///
template<typename T>
struct Identity
{
    using type = T;
};

///
/// @brief same as boost::all_of
///
/// Typically used by a std::vector<T>
///
template<template<typename...> class V, typename T, typename... Rest>
inline bool all(const V<T, Rest...> &v,
                typename Identity<std::function<bool(const T&)>>::type f)
{
    return std::all_of(v.begin(), v.end(), f);
}

///
/// @brief same as boost:all_of
///
/// Typically used by a std::array<T, N>
///
template<template<typename T, std::size_t N> class V, typename T, std::size_t N>
inline bool all(const V<T, N> &v,
                typename Identity<std::function<bool(const T&)>>::type f)
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
/// Typically used by a std::vector<T>
///
template<template<typename...> class V, class T, typename... Rest>
inline bool any(const V<T, Rest...> &v,
                typename Identity<std::function<bool(const T&)>>::type f)
{
    return std::any_of(v.begin(), v.end(), f);
}

///
/// @brief same as boost::any_of
///
/// Typically used by a std::array<T, N>
///
template<template<typename T, std::size_t N> class V, typename T, std::size_t N>
inline bool any(const V<T, N> &v,
                typename Identity<std::function<bool(const T&)>>::type f)
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
/// Typically used by a std::vector<T>
///
template<template<typename...> class V, typename T, typename... Rest>
inline bool none(const V<T, Rest...> &v,
                typename Identity<std::function<bool(const T&)>>::type f)
{
    return std::none_of(v.begin(), v.end(), f);
}

///
/// @brief same as boost::none_of
///
/// Typically used by a std::array<T, N>
///
template<template<typename T, std::size_t N> class V, typename T, std::size_t N>
inline bool none(const V<T, N> &v,
                typename Identity<std::function<bool(const T&)>>::type f)
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
/// Typically used by a std::vector<T>
///
template<template<typename...> class V, typename T, typename... Rest>
inline bool has(const V<T, Rest...> &v, T e)
{
    return std::find(v.begin(), v.end(), e) != v.end();
}

///
/// @brief same as boost::any_of_equal
///
/// Typically used by a std::array<T, N>
///
template<template<typename T, std::size_t N> class V, typename T, std::size_t N>
inline bool has(const V<T, N> &v, T e)
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
/// @brief alias for 'std::cout << arg << std::endl;', just save typing
///
template <typename T>
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



#endif // UTIL_H
