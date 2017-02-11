#ifndef SAKI_GIRLS_RINKAI_H
#define SAKI_GIRLS_RINKAI_H

#include "girl.h"



namespace saki
{



class Huiyu : public Girl
{
public:
    GIRL_CTORS(Huiyu)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



}



#endif // SAKI_GIRLS_RINKAI_H


