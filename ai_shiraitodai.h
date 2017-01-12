#ifndef AI_SHIRAITODAI_H
#define AI_SHIRAITODAI_H

#include "ai.h"



namespace saki
{



class AiTakami : public Ai
{
public:
    AI_CTORS(AiTakami)
    int happy(const TableView &view, int iter, const Action &action) override;
};



class AiAwai : public Ai
{
public:
    AI_CTORS(AiAwai)
    Action forward(const TableView &view) override;
};



} // namespace saki



#endif // AI_SHIRAITODAI_H


