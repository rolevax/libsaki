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

    void append(C34 head);

private:
    util::Stactor<C34, 14> mHeads;
};



inline std::ostream &operator<<(std::ostream &os, const Parsed &p)
{
    return os << p.heads();
}



} // namespace saki



#endif // SAKI_PARSED_H


