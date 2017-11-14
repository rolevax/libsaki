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


