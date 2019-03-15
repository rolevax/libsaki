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

inline bool isZ(T34 t)
{
    return t.isZ();
}

inline bool isYao(T34 t)
{
    return t.isYao();
}

inline bool isNum19(T34 t)
{
    return t.isNum19();
}



} // namepace saki



#endif // SAKI_FORM_COMMON_H
