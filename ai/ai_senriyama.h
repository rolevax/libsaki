#ifndef SAKI_AI_SENRIYAMA_H
#define SAKI_AI_SENRIYAMA_H

#include "ai.h"



namespace saki
{



class AiToki : public Ai
{
public:
    AI_CTORS(AiToki)
    Action thinkIrs(const TableView &view) override;
};



} // namespace saki



#endif // SAKI_AI_SENRIYAMA_H
