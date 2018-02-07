#ifndef SAKI_AI_KIYOSUMI_NODOKA_H
#define SAKI_AI_KIYOSUMI_NODOKA_H

#include "ai.h"



namespace saki
{



class AiNodoka : public Ai
{
public:
    Action think(const TableView &view, Limits &limits) override;
};



} // namespace saki



#endif // SAKI_AI_KIYOSUMI_NODOKA_H
