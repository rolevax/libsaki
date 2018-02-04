#ifndef SAKI_AI_ACHIGA_KURO_H
#define SAKI_AI_ACHIGA_KURO_H

#include "ai.h"



namespace saki
{



class AiKuro : public Ai
{
public:
    AI_CTORS(AiKuro)
    Action think(const TableView &view, Limits &limits) override;
};



} // namespace saki



#endif // SAKI_AI_ACHIGA_KURO_H
