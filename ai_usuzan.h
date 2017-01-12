#ifndef AI_USUZAN_H
#define AI_USUZAN_H

#include "ai.h"



namespace saki
{



class AiSawaya : public Ai
{
public:
    AI_CTORS(AiSawaya)
    Action forward(const TableView &view) override;
    int happy(const TableView &view, int iter, const Action &action) override;
};



} // namespace saki



#endif // AI_USUZAN_H


