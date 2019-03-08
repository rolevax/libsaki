#ifndef SAKI_PARSED_VIEW_H
#define SAKI_PARSED_VIEW_H

#include "parsed.h"



namespace saki
{



class ParsedView4Ready
{
public:
    explicit ParsedView4Ready(const Parsed4 &parsed);

    const Parsed4 &getParsed() const;

    util::Stactor<T34, 2> getIsoriders() const;
    util::Stactor<T34, 2> get1s() const;
    util::Stactor<C34, 2> get2s() const;

private:
    const Parsed4 &mParsed;
};

class ParsedView4Step1
{
public:
    explicit ParsedView4Step1(const Parsed4 &parsed);
    util::Stactor<T34, 2> getFrees() const;

private:
    const Parsed4 &mParsed;
};



} // namespace saki



#endif // SAKI_PARSED_VIEW_H
