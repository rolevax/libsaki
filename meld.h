#ifndef MELD_H
#define MELD_H

#include "tile.h"

#include <vector>
#include <initializer_list>
#include <ostream>
#include <algorithm>



namespace saki
{



class M37
{
public:
    enum class Type
    {
        // *** SYNC with string_enum.cpp ***
        CHII, PON, DAIMINKAN, ANKAN, KAKAN
    };

    M37(const M37 &copy) = default;
    M37 &operator=(const M37 &assign) = default;
    ~M37() = default;

    static M37 chii(const T37 &l, const T37 &m, const T37 &r, int lay)
    {
        M37 bark(Type::CHII, lay, { l, m, r });
        return bark;
    }

    static M37 pon(const T37 &l, const T37 &m, const T37 &r, int lay)
    {
        assert(lay == 0 || lay == 1 || lay == 2);
        M37 bark(Type::PON, lay, { l, m, r });
        return bark;
    }

    static M37 daiminkan(const T37 &l, const T37 &m, const T37 &r, const T37 &x,
                         int lay)
    {
        assert(lay == 0 || lay == 1 || lay == 2);
        M37 bark(Type::DAIMINKAN, lay, { l, m, r, x });
        return bark;
    }

    static M37 ankan(const T37 &l, const T37 &m, const T37 &r, const T37 &x)
    {
        M37 bark(Type::ANKAN, -1, { l, m, r, x });
        return bark;
    }

    void kakan(const T37 &x)
    {
        assert(mType == Type::PON);
        mTiles.emplace_back(x);
        mType = Type::KAKAN;
    }

    Type type() const
    {
        return mType;
    }

    int layIndex() const
    {
        return mLay;
    }

    const std::vector<T37> &tiles() const
    {
        return mTiles;
    }

    const T37 &operator[](int i) const
    {
        return mTiles[i];
    }

    bool isCpdmk() const
    {
        return mType == Type::CHII
                || mType == Type::PON
                || mType == Type::DAIMINKAN;
    }

    bool isKan() const
    {
        return mType == Type::DAIMINKAN
                || mType == Type::ANKAN
                || mType == Type::KAKAN;
    }

    bool has(T34 t) const
    {
        switch (mType) {
        case Type::CHII:
            return t == mTiles[0] || t == mTiles[1] || t == mTiles[2];
        default:
            return t == mTiles[0];
        }
    }

private:
    M37(Type type, int lay, std::initializer_list<T37> tiles)
        : mType(type)
        , mLay(lay)
        , mTiles(tiles)
    {
    }

private:
    Type mType;
    int mLay;
    std::vector<T37> mTiles;
};

inline int operator%(T34 ind, const M37 &m)
{
    const auto &ts = m.tiles();
    return std::count(ts.begin(), ts.end(), ind.dora());
}

inline std::ostream &operator<<(std::ostream &os, const M37 &m)
{
    for (const T37 &t : m.tiles()) {
        os << (t.isAka5() ? 0 : t.val());
    }
    os << T34::charOf(m[0].suit());
    return os;
}



} // namespace saki



#endif // MELD_H
