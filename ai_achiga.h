#ifndef SAKI_AI_ACHIGA_H
#define SAKI_AI_ACHIGA_H

#include "ai.h"



namespace saki
{



class AiKuro : public Ai
{
public:
    AI_CTORS(AiKuro)
    int happy(const TableView &view, int iter, const Action &action) override;
};



} // namespace saki



#endif // SAKI_AI_ACHIGA_H


