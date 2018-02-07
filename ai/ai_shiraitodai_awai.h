#ifndef SAKI_AI_SHIRAITODAI_AWAI_H
#define SAKI_AI_SHIRAITODAI_AWAI_H

#include "ai.h"



namespace saki
{



class AiAwai : public Ai
{
public:
    Action thinkIrs(const TableView &view) override;
};



} // namespace saki



#endif // SAKI_AI_SHIRAITODAI_AWAI_H
