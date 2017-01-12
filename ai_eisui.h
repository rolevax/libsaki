#ifndef AI_EISUI_H
#define AI_EISUI_H

#include "ai.h"



namespace saki
{



class AiKasumi : public Ai
{
public:
    AI_CTORS(AiKasumi)
    Action forward(const TableView &view) override;
};



} // namespace saki



#endif // AI_EISUI_H


