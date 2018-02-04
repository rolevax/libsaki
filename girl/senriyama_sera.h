#ifndef SAKI_GIRL_SENRIYAMA_SERA_H
#define SAKI_GIRL_SENRIYAMA_SERA_H

#include "../table/girl.h"



namespace saki
{



class Sera : public Girl
{
public:
    GIRL_CTORS(Sera)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



} // namespace saki



#endif // SAKI_GIRL_SENRIYAMA_SERA_H
