#ifndef SAKI_GIRLS_OTHER_H
#define SAKI_GIRLS_OTHER_H

#include "girl.h"



namespace saki
{



class Kazue : public Girl
{
public:
    GIRL_CTORS(Kazue)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



class Uta : public Girl
{
public:
    GIRL_CTORS(Uta)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



class Yui : public Girl
{
public:
    GIRL_CTORS(Yui)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;

};



} // namespace saki



#endif // SAKI_GIRLS_OTHER_H


