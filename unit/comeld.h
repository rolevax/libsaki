#ifndef SAKI_COMELD_H
#define SAKI_COMELD_H

#include "tile.h"



namespace saki
{



class C34
{
public:
    enum class Type
    {
        SEQ, TRI, BIFACE, CLAMP, SIDE, PAIR, FREE
    };

    /// \brief Garbage value
    C34()
    {
    }

    C34(Type type, T34 head)
        : mType(type)
        , mHead(head)
    {
#ifndef NDEBUG
        int v = head.val();
        bool num = head.isNum();
        assert((type != Type::SEQ || (num && v <= 7))
               && (type != Type::BIFACE || (num && 2 <= v && v <= 7))
               && (type != Type::CLAMP || (num && v <= 7))
               && (type != Type::SIDE || (num && (v == 1 || v == 8))));
#endif
    }

    C34(const C34 &copy) = default;
    C34 &operator=(const C34 &assign) = default;
    ~C34() = default;

    Type type() const
    {
        return mType;
    }

    T34 head() const
    {
        return mHead;
    }

    int work() const
    {
        switch (mType) {
        case Type::SEQ:
        case Type::TRI:
            return 2;
        case Type::BIFACE:
        case Type::CLAMP:
        case Type::SIDE:
        case Type::PAIR:
            return 1;
        default:
            return 0;
        }
    }

    util::Stactor<T34, 3> t34s() const
    {
        // save typing
        using S = util::Stactor<T34, 3>;
        const T34 h = mHead;

        switch (mType) {
        case Type::SEQ:
            return S { h, h.next(), h.nnext() };
        case Type::TRI:
            return S { h, h, h };
        case Type::BIFACE:
            return S { h, h.next() };
        case Type::CLAMP:
            return S { h, h.nnext() };
        case Type::SIDE:
            return S { h, h.next() };
        case Type::PAIR:
            return S { h, h };
        case Type::FREE:
            return S { h };
        default:
            unreached("illegal c34 type");
        }
    }

    bool has(T34 t) const
    {
        const auto &ts = t34s();
        return std::find(ts.begin(), ts.end(), t) != ts.end();
    }

    bool is3() const
    {
        return mType == Type::SEQ || mType == Type::TRI;
    }

    bool is2() const
    {
        return isSeq2() || mType == Type::PAIR;
    }

    bool isSeq2() const
    {
        return mType == Type::BIFACE || mType == Type::CLAMP || mType == Type::SIDE;
    }

    util::Stactor<T34, 3> tilesTo(const C34 &that) const
    {
        return tilesTo(that.t34s());
    }

    util::Stactor<T34, 3> tilesTo(const util::Stactor<T34, 3> &tar) const
    {
        util::Stactor<T34, 3> res;
        auto need = [this](T34 t) { return !has(t); };
        std::copy_if(tar.begin(), tar.end(), std::back_inserter(res), need);
        return res;
    }

    bool operator==(const C34 &that) const
    {
        return mType == that.mType && mHead == that.mHead;
    }

    bool operator!=(const C34 &that) const
    {
        return !(*this == that);
    }

    bool operator<(const C34 &that) const
    {
        if (mType != that.mType) {
            int a = static_cast<int>(mType);
            int b = static_cast<int>(that.mType);
            return a < b;
        }

        return mHead < that.mHead;
    }

private:
    Type mType;
    T34 mHead;
};



inline std::ostream &operator<<(std::ostream &os, C34 c)
{
    int v = c.head().val();
    char s = T34::charOf(c.head().suit());
    switch (c.type()) {
    case C34::Type::SEQ:
        return os << v << (v + 1) << (v + 2) << s;
    case C34::Type::TRI:
        return os << v << v << v << s;
    case C34::Type::BIFACE:
        return os << v << (v + 1) << s;
    case C34::Type::CLAMP:
        return os << v << (v + 2) << s;
    case C34::Type::SIDE:
        return os << v << (v + 1) << s;
    case C34::Type::PAIR:
        return os << v << v << s;
    case C34::Type::FREE:
        return os << c.head();
    default:
        unreached("os << c34");
    }
}

template<size_t MAX>
inline std::ostream &operator<<(std::ostream &os, const util::Stactor<C34, MAX> &cs)
{
    bool first = true;

    for (C34 c : cs) {
        if (first)
            first = false;
        else
            os << ' ';
        os << c;
    }

    return os;
}



} // namespace saki



#endif // SAKI_COMELD_H


