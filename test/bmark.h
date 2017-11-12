#ifndef SAKI_BMARK_H
#define SAKI_BMARK_H

#include "../girl/girl.h"



namespace saki
{



class Bmark : public TableObserver
{
public:
    static void test(Girl::Id girlId);

    Bmark() = default;
    Bmark(const Bmark &copy) = default;
    Bmark &operator=(const Bmark &assign) = default;
    virtual ~Bmark() = default;

    void onTableEnded(const std::array<Who, 4> &rank, const std::array<int, 4> &scores);
};



}



#endif // SAKI_BMARK_H


