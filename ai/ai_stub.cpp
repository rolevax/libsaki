#include "ai_stub.h"



namespace saki
{



Action AiStub::think(const TableView &view, Ai::Limits &limits)
{
    (void) limits;
    return Ai::placeHolder(view);
}



} // namespace saki
