#ifndef SAKI_FORM_COMMON_H
#define SAKI_FORM_COMMON_H

#include "../unit/tile.h"



namespace saki
{



inline bool isY(T34 t)
{
    return t.suit() == Suit::Y;
}

inline bool isF(T34 t)
{
    return t.suit() == Suit::F;
}



} // namepace saki



#endif // SAKI_FORM_COMMON_H
