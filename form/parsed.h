#ifndef SAKI_PARSED_H
#define SAKI_PARSED_H

#include "../unit/comeld.h"



namespace saki
{



class Parsed
{
public:
    using Heads = util::Stactor<C34, 14>;

    static int workOfHeads(const Heads &heads);

    explicit Parsed(const Heads &heads);

    const Heads &heads() const;
    int step4(int barkCt) const;
    std::bitset<34> effA4() const;

    util::Stactor<T34, 9> claim3sk() const;

    bool operator==(const Parsed &that) const;

private:
    util::Stactor<T34, 9> minTilesTo(const std::array<C34, 3> &cs) const;
    util::Stactor<T34, 3> minTilesTo(const C34 &cs) const;

private:
    Heads mHeads;
};



inline std::ostream &operator<<(std::ostream &os, const Parsed &p)
{
    return os << p.heads();
}



class Parseds
{
public:
    using Container = std::vector<Parsed>;

    explicit Parseds(Container &&parseds, int barkCt);

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



} // namespace saki



#endif // SAKI_PARSED_H
