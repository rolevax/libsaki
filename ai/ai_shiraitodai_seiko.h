#ifndef SAKI_AI_SHIRAITODAI_SEIKO_H
#define SAKI_AI_SHIRAITODAI_SEIKO_H

#include "ai.h"



namespace saki
{



class AiSeiko : public Ai
{
public:
    Action think(const TableView &view, Limits &limits) override;
};



} // namespace saki



#endif // SAKI_AI_SHIRAITODAI_SEIKO_H
