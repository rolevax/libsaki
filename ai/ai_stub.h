#ifndef SAKI_AI_STUB_H
#define SAKI_AI_STUB_H

#include "ai.h"



namespace saki
{



class AiStub : public Ai
{
public:
    explicit AiStub(Who who);
    Action think(const TableView &view, Limits &limits) override;
};



} // namespace saki



#endif // SAKI_AI_STUB_H


