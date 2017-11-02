#ifndef SAKI_COMELD_H
#define SAKI_COMELD_H

#include "tile.h"



namespace saki
{



class C34
{
public:
    enum class Type
    {
        BIFACE, CLAMP, SIDE, PAIR
    };

    C34(Type type, T34 head)
        : mType(type)
        , mHead(head)
    {
    }

private:
    Type mType;
    T34 mHead;
};



}



#endif // SAKI_COMELD_H


