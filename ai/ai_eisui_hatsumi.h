#ifndef SAKI_AI_EISUI_HATSUMI_H
#define SAKI_AI_EISUI_HATSUMI_H

#include "ai.h"



namespace saki
{



class AiHatsumi : public Ai
{
public:
    Action think(const TableView &view, Limits &limits) override;
};



} // namespace saki



#endif // SAKI_AI_EISUI_HATSUMI_H
