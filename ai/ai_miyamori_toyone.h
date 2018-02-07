#ifndef SAKI_AI_MIYAMORI_TOYONE_H
#define SAKI_AI_MIYAMORI_TOYONE_H

#include "ai.h"



namespace saki
{



class AiToyone : public Ai
{
public:
    Action think(const TableView &view, Limits &limits) override;
};



} // namespace saki



#endif // SAKI_AI_MIYAMORI_TOYONE_H
