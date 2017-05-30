#ifndef SAKI_POINTINFO_H
#define SAKI_POINTINFO_H

#include "tile_count.h"



namespace saki
{



struct PointInfo
{
    bool ippatsu = false;
    bool bless = false;
    bool duringKan = false;
    bool emptyMount = false;
    int riichi = 0;
    int roundWind = 0; // not any wind
    int selfWind = 0; // not any wind
    int extraRound = 0;
};

struct RuleInfo
{
    // *** SYNC with json converting (create + read) ***
    bool fly = true;
    bool headJump = true;
    bool nagashimangan = true;
    bool ippatsu = true;
    bool uradora = true;
    bool kandora = true;
    bool daiminkanPao = true;
    TileCount::AkadoraCount akadora = TileCount::AKADORA4;
    int hill = 20000;
    int returnLevel = 30000;
    int roundLimit = 8;
};



} // namespace saki



#endif // SAKI_POINTINFO_H
