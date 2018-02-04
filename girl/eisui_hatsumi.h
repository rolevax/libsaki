#ifndef SAKI_GIRL_EISUI_HATSUMI_H
#define SAKI_GIRL_EISUI_HATSUMI_H

#include "../table/girl.h"



namespace saki
{



class Hatsumi : public Girl
{
public:
    GIRL_CTORS(Hatsumi)

    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;
};



} // namespace saki



#endif // SAKI_GIRL_EISUI_HATSUMI_H
