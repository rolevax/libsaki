#ifndef SAKI_AI_SHIRAITODAI_TAKAMI_H
#define SAKI_AI_SHIRAITODAI_TAKAMI_H

#include "ai.h"



namespace saki
{



class AiTakami : public Ai
{
public:
    AI_CTORS(AiTakami)
    Action think(const TableView &view, Limits &limits) override;
};



} // namespace saki



#endif // SAKI_AI_SHIRAITODAI_TAKAMI_H
