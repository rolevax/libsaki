#ifndef SAKI_AI_EISUI_H
#define SAKI_AI_EISUI_H

#include "ai.h"



namespace saki
{



class AiHatsumi : public Ai
{
public:
    AI_CTORS(AiHatsumi)
    int happy(const TableView &view, int iter, const Action &action) override;
};

class AiKasumi : public Ai
{
public:
    AI_CTORS(AiKasumi)
    Action forward(const TableView &view) override;
};



} // namespace saki



#endif // SAKI_AI_EISUI_H


