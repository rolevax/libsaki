#ifndef SAKI_AI_EISUI_KASUMI_H
#define SAKI_AI_EISUI_KASUMI_H

#include "ai.h"



namespace saki
{



class AiKasumi : public Ai
{
public:
    AI_CTORS(AiKasumi)
    Action thinkIrs(const TableView &view) override;
};



} // namespace saki



#endif // SAKI_AI_EISUI_KASUMI_H
