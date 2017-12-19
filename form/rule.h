#ifndef SAKI_RULE_H
#define SAKI_RULE_H

#include "tile_count.h"



namespace saki
{



struct Rule
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



#endif // SAKI_RULE_H
