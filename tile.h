#ifndef SAKI_TILE_H
#define SAKI_TILE_H

#include "assume.h"

#include <array>
#include <vector>
#include <ostream>
#include <cctype>
#include <cstring>

#define valid_id34(i) (0 <= i && i < 34)



namespace saki
{



enum class Suit { M = 0, P = 1, S = 2, F = 3, Y = 4 };

enum class Wait { NONE, ISORIDE, BIBUMP, CLAMP, SIDE, BIFACE };

inline int _tid34(Suit suit, int val)
{
    assert(1 <= val && val <= 9);
    assume_opt_out(1 <= val && val <= 9);

    return suit == Suit::F ? 27 + val - 1
         : suit == Suit::Y ? 31 + val - 1
         : 9 * static_cast<int>(suit) + val - 1;
}

constexpr int _tid34(int val, Suit suit)
{
    return suit == Suit::F ? 27 + val - 1
         : suit == Suit::Y ? 31 + val - 1
         : 9 * static_cast<int>(suit) + val - 1;
}

class T34
{
public:
    static char charOf(Suit s)
    {
        switch(s) {
        case Suit::M: return 'm';
        case Suit::P: return 'p';
        case Suit::S: return 's';
        case Suit::F: return 'f';
        case Suit::Y: return 'y';
        default: unreached("T34::charOf");
        }
    }

    static Suit suitOf(char c)
    {
        switch(std::tolower(c)) {
        case 'm': return Suit::M;
        case 'p': return Suit::P;
        case 's': return Suit::S;
        case 'f': return Suit::F;
        case 'y': return Suit::Y;
        default: unreached("Tile::charToSuit() error");
        }
    }

    /// \brief Garbage value
    T34()
#ifndef NDEBUG
        : mInitialized(false)
#endif
    {
    }

    explicit T34(int id34)
        : mId34(id34)
    {
        assert(valid_id34(mId34));
        assume_opt_out(valid_id34(mId34));
    }

    explicit T34(Suit suit, int val)
        : mId34(_tid34(suit, val))
    {
        assert(valid_id34(mId34));
        assume_opt_out(valid_id34(mId34));
    }

    constexpr explicit T34(int val, Suit suit)
        : mId34(_tid34(val, suit))
    {
    }

    T34(const T34 &copy) = default;
    T34 &operator=(const T34 &assign) = default;
    ~T34() = default;

    int id34() const
    {
#ifndef NDEBUG
        assert(mInitialized);
#endif
        return mId34;
    }

    Suit suit() const
    {
#ifndef NDEBUG
        assert(mInitialized);
#endif
        if (0 <= mId34 && mId34 < 9)
            return Suit::M;
        if (9 <= mId34 && mId34 < 18)
            return Suit::P;
        if (18 <= mId34 && mId34 < 27)
            return Suit::S;
        if (27 <= mId34 && mId34 < 31)
            return Suit::F;
        if (31 <= mId34 && mId34 < 34)
            return Suit::Y;

        unreached("T34::suit(): invalid");
    }

    int val() const
    {
#ifndef NDEBUG
        assert(mInitialized);
#endif
        if (0 <= mId34 && mId34 < 27)
            return mId34 % 9 + 1;
        if (27 <= mId34 && mId34 < 31)
            return mId34 - 27 + 1;
        if (31 <= mId34 && mId34 < 34)
            return mId34 - 31 + 1;

        unreached("T34::val(): invalid");
    }

    const char *str() const
    {
        static const std::array<const char*, 34> STRS {
            "1m", "2m", "3m", "4m", "5m", "6m", "7m", "8m", "9m",
            "1p", "2p", "3p", "4p", "5p", "6p", "7p", "8p", "9p",
            "1s", "2s", "3s", "4s", "5s", "6s", "7s", "8s", "9s",
            "1f", "2f", "3f", "4f",
            "1y", "2y", "3y"
        };

        return STRS[id34()];
    }


    bool isZ() const
    {
        return suit() == Suit::F || suit() == Suit::Y;
    }

    bool isNum() const
    {
        return !isZ();
    }

    bool isNum19() const
    {
        return isNum() && (val() == 1 || val() == 9);
    }

    bool isYao() const
    {
        return isZ() || val() == 1 || val() == 9;
    }

    bool isYakuhai(int selfWind, int roundWind) const
    {
        assert(1 <= selfWind && selfWind <= 4);
        assert(1 <= roundWind && roundWind <= 4);

        return suit() == Suit::Y
                || (suit() == Suit::F && (val() == selfWind || val() == roundWind));
    }


    bool operator==(T34 rhs) const
    {
        return id34() == rhs.id34();
    }

    bool operator!=(T34 rhs) const
    {
        return !(*this == rhs);
    }

    bool operator<(T34 rhs) const
    {
        return id34() < rhs.id34();
    }

    bool operator|(T34 up) const
    {
        return isNum() && suit() == up.suit() && val() + 1 == up.val();
    }

    bool operator||(T34 up) const
    {
        return isNum() && suit() == up.suit() && val() + 2 == up.val();
    }

    bool operator^(T34 up) const
    {
        return isNum() && suit() == up.suit() && val() + 3 == up.val();
    }

    bool operator%(T34 dora) const
    {
        // since max of val() ranges from 1 to period (not period - 1),
        // there is nothing to worry about
        return suit() == dora.suit() && val() % period() + 1 == dora.val();
    }

    T34 prev() const
    {
        assert(isNum() && val() >= 2);
        assume_opt_out(isNum() && val() >= 2);
        return T34(suit(), val() - 1);
    }

    T34 pprev() const
    {
        assert(isNum() && val() >= 3);
        assume_opt_out(isNum() && val() >= 3);
        return T34(suit(), val() - 2);
    }

    T34 next() const
    {
        assert(isNum() && 1 <= val() && val() <= 8);
        assume_opt_out(isNum() && 1 <= val() && val() <= 8);
        return T34(suit(), val() + 1);
    }

    T34 nnext() const
    {
        assert(isNum() && 1 <= val() && val() <= 7);
        assume_opt_out(isNum() && 1 <= val() && val() <= 7);
        return T34(suit(), val() + 2);
    }

    T34 dora() const
    {
        return T34(suit(), val() % period() + 1);
    }

    T34 indicator() const
    {
        // first '-1': from 1-index to 0-index
        // second '+period': prevent being minus
        // third '-1': previou tile
        // last '+1': from 0-index to 1-index
        return T34(suit(), (val() - 1 + period() - 1) % period() + 1);
    }

    Wait waitAsSequence(T34 pick) const
    {
        if (suit() != pick.suit())
            return Wait::NONE;

        int va = val();
        int vb = pick.val();

        // 123/3 789/7
        if ((va == 1 && vb == 3) || (va == 7 && vb == 7))
            return Wait::SIDE;

        // xyz/y
        if (va + 1 == vb)
            return Wait::CLAMP;

        // xyz/x xyz/z
        if (va == vb || va + 2 == vb)
            return Wait::BIFACE;

        return Wait::NONE;
    }

private:
    int period() const
    {
        switch (suit()) {
        case Suit::M:
        case Suit::P:
        case Suit::S:
            return 9;
        case Suit::F:
            return 4;
        case Suit::Y:
            return 3;
        default:
            unreached("T34::period");
        }
    }

private:
    int mId34;
#ifndef NDEBUG
    bool mInitialized = true;
#endif
};

inline std::ostream &operator<<(std::ostream &os, T34 t)
{
    return os << t.str();
}

inline std::ostream &operator<<(std::ostream &os, const std::vector<T34> &ts)
{
    for (size_t i = 0; i < ts.size(); i++) {
        os << ts[i].val();
        if (i < ts.size() - 1
                && ts[i].suit() != ts[i + 1].suit())
            os << T34::charOf(ts[i].suit());
    }

    os << T34::charOf(ts.back().suit());

    return os;
}

template<size_t N>
inline std::ostream &operator<<(std::ostream &os, const std::array<T34, N> &ts)
{
    for (size_t i = 0; i < ts.size(); i++) {
        os << ts[i].val();
        if (i < ts.size() - 1
                && ts[i].suit() != ts[i + 1].suit())
            os << T34::charOf(ts[i].suit());
    }

    os << T34::charOf(ts.back().suit());

    return os;
}



class T37 : public T34
{
public:
    /// \brief Garbage value
    T37()
        : T34()
    {
    }

    T37(const T37 &copy) = default;

    explicit T37(int id34)
        : T34(id34)
        , mAka5(false)
    {
    }

    explicit T37(Suit suit, int val)
        : T34(suit, val == 0 ? 5 : val)
        , mAka5(val == 0)
    {
    }

    constexpr explicit T37(int val, Suit suit)
        : T34(val == 0 ? 5 : val, suit)
        , mAka5(val == 0)
    {
    }

    explicit T37(const char *str)
        : T34(suitOf(str[1]), str[0] == '0' ? 5 : str[0] - '0')
        , mAka5(str[0] == '0')
    {
        assert(str[2] == '\0');
    }

    ///
    /// \brief String representation of the tile
    /// \return "0m", "0p", "0s" when the tile is an aka5, T34::str() otherwise.
    ///
    /// This function is not virtual
    ///
    const char *str() const
    {
        if (isAka5()) {
            switch(suit()) {
            case Suit::M: return "0m";
            case Suit::P: return "0p";
            case Suit::S: return "0s";
            default: unreached("T37::str(): aka5 but z");
            }
        } else {
            return T34::str();
        }
    }

    bool isAka5() const
    {
        return mAka5;
    }

    bool looksSame(const T37 &rhs) const
    {
        return id34() == rhs.id34() && mAka5 == rhs.mAka5;
    }

    T37 toAka5() const
    {
        assert(val() == 5);
        assume_opt_out(val() == 5);
        return T37(id34(), true);
    }

    T37 toInverse5() const
    {
        assert(val() == 5);
        assume_opt_out(val() == 5);
        return T37(id34(), !mAka5);
    }

private:
    explicit T37(int id34, bool aka5)
        : T34(id34)
        , mAka5(aka5)
    {
    }

private:
    bool mAka5;
};

inline int operator%(const std::vector<T37> &inds, const T37 &d)
{
    int s = 0;
    for (const T37 &ind : inds)
        s += (ind % d);
    return s;
}

inline std::ostream &operator<<(std::ostream &os, const T37 &t)
{
    return os << t.str();
}

inline std::ostream &operator<<(std::ostream &os, const std::vector<T37> &ts)
{
    for (size_t i = 0; i < ts.size(); i++) {
        os << (ts[i].isAka5() ? 0 : ts[i].val());
        if (i < ts.size() - 1
                && ts[i].suit() != ts[i + 1].suit())
            os << T34::charOf(ts[i].suit());
    }

    os << T34::charOf(ts.back().suit());

    return os;
}



namespace tiles34
{



constexpr T34 operator""_m(unsigned long long val)
{
    return T34(val, Suit::M);
}

constexpr T34 operator""_p(unsigned long long val)
{
    return T34(val, Suit::P);
}

constexpr T34 operator""_s(unsigned long long val)
{
    return T34(val, Suit::S);
}

constexpr T34 operator""_f(unsigned long long val)
{
    return T34(val, Suit::F);
}

constexpr T34 operator""_y(unsigned long long val)
{
    return T34(val, Suit::Y);
}

const std::array<T34, 13> YAO13
{
    1_m, 9_m, 1_p, 9_p, 1_s, 9_s,
    1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y
};

const std::array<T34, 7> Z7
{
    1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y
};



} // namespace tiles34



namespace tiles37
{



constexpr T37 operator""_m(unsigned long long val)
{
    return T37(val, Suit::M);
}

constexpr T37 operator""_p(unsigned long long val)
{
    return T37(val, Suit::P);
}

constexpr T37 operator""_s(unsigned long long val)
{
    return T37(val, Suit::S);
}

constexpr T37 operator""_f(unsigned long long val)
{
    return T37(val, Suit::F);
}

constexpr T37 operator""_y(unsigned long long val)
{
    return T37(val, Suit::Y);
}

const std::array<T37, 37> ORDER37
{
    1_m, 2_m, 3_m, 4_m, 0_m, 5_m, 6_m, 7_m, 8_m, 9_m,
    1_p, 2_p, 3_p, 4_p, 0_p, 5_p, 6_p, 7_p, 8_p, 9_p,
    1_s, 2_s, 3_s, 4_s, 0_s, 5_s, 6_s, 7_s, 8_s, 9_s,
    1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y
};



} // namespace tiles37



} // namespace saki


#endif // SAKI_TILE_H


