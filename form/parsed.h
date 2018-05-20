#ifndef SAKI_PARSED_H
#define SAKI_PARSED_H

#include "../unit/comeld.h"



namespace saki
{



class Parsed4
{
public:
    using Heads = util::Stactor<C34, 14>;

    static int workOfHeads(const Heads &heads);

    explicit Parsed4(const Heads &heads);

    const Heads &heads() const;

    int step4(int barkCt) const;
    std::bitset<34> effA4Set() const;

    util::Stactor<T34, 9> claim3sk() const;

    bool operator==(const Parsed4 &that) const;

private:
    util::Stactor<T34, 9> minTilesTo(const std::array<C34, 3> &cs) const;
    util::Stactor<T34, 3> minTilesTo(const C34 &cs) const;

private:
    Heads mHeads;
};



inline std::ostream &operator<<(std::ostream &os, const Parsed4 &p)
{
    return os << p.heads();
}



class Parsed4s
{
public:
    using Container = std::vector<Parsed4>;

    explicit Parsed4s(Container &&parseds, int barkCt);

    const Container &data() const;
    int size() const;
    Container::const_iterator begin() const;
    Container::const_iterator end() const;

    util::Stactor<T34, 34> effA4() const;
    std::bitset<34> effA4Set() const;

private:
    Container mParseds;
    int mBarkCt;
};



class Parsed7
{
public:
    explicit Parsed7(const std::bitset<34> &plurals, const std::bitset<34> &floats);

    int step7() const;
    std::bitset<34> effA7Set() const;

private:
    std::bitset<34> mPlurals;
    std::bitset<34> mFloats;
    int mNeedKind;
};



class Parsed13
{
public:
    explicit Parsed13(const std::bitset<34> &yaos, bool hasYaoPair);

    int step13() const;
    std::bitset<34> effA13Set() const;

private:
    std::bitset<34> mYaos;
    bool mHasYaoPair;
};



} // namespace saki



#endif // SAKI_PARSED_H
