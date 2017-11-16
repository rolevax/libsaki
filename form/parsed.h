#ifndef SAKI_PARSED_H
#define SAKI_PARSED_H

#include "../unit/comeld.h"



namespace saki
{



class Parsed
{
public:
    Parsed() = default;

    const util::Stactor<C34, 14> &heads() const;
    int work() const;

    util::Stactor<T34, 9> claim3sk() const;

    bool operator==(const Parsed &that) const;

    void append(C34 head);
    void sort();

private:
    util::Stactor<T34, 9> minTilesTo(const std::array<C34, 3> &cs) const;
    util::Stactor<T34, 3> minTilesTo(const C34 &cs) const;

private:
    util::Stactor<C34, 14> mHeads;
};



inline std::ostream &operator<<(std::ostream &os, const Parsed &p)
{
    return os << p.heads();
}



} // namespace saki



#endif // SAKI_PARSED_H


