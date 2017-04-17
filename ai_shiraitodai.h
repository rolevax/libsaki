#ifndef SAKI_AI_SHIRAITODAI_H
#define SAKI_AI_SHIRAITODAI_H

#include "ai.h"



namespace saki
{



class AiTakami : public Ai
{
public:
    AI_CTORS(AiTakami)
    Action think(const TableView &view, const std::vector<Action> &choices) override;
};



class AiSeiko : public Ai
{
public:
    AI_CTORS(AiSeiko)
    Action think(const TableView &view, const std::vector<Action> &choices) override;
};



class AiAwai : public Ai
{
public:
    AI_CTORS(AiAwai)
    Action forward(const TableView &view) override;
};



} // namespace saki



#endif // SAKI_AI_SHIRAITODAI_H


