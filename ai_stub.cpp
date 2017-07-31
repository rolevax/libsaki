#include "ai_stub.h"



namespace saki
{



AiStub::AiStub(Who who)
    : Ai(who)
{
}

Action AiStub::think(const TableView &view, Ai::Limits &limits)
{
    (void) limits;
    return Ai::placeHolder(view);
}



} // namespace saki


