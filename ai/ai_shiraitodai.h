#ifndef SAKI_AI_SHIRAITODAI_H
#define SAKI_AI_SHIRAITODAI_H

#include "ai.h"



namespace saki
{



class AiTakami : public Ai
{
public:
    AI_CTORS(AiTakami)
    Action think(const TableView &view, Limits &limits) override;
};



class AiSeiko : public Ai
{
public:
    AI_CTORS(AiSeiko)
    Action think(const TableView &view, Limits &limits) override;
};



class AiAwai : public Ai
{
public:
    AI_CTORS(AiAwai)
    Action thinkIrs(const TableView &view) override;
};



} // namespace saki



#endif // SAKI_AI_SHIRAITODAI_H
