#ifndef SAKI_PARSED_H
#define SAKI_PARSED_H

#include "../unit/comeld.h"

#include <bitset>



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



} // namespace saki



#endif // SAKI_PARSED_H
