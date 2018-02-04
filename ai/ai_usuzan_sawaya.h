#ifndef SAKI_AI_USUZAN_SAWAYA_H
#define SAKI_AI_USUZAN_SAWAYA_H

#include "ai.h"



namespace saki
{



class AiSawaya : public Ai
{
public:
    AI_CTORS(AiSawaya)
    Action thinkIrs(const TableView &view) override;
    Action think(const TableView &view, Limits &limits) override;
};



} // namespace saki



#endif // SAKI_AI_USUZAN_SAWAYA_H
