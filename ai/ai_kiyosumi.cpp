#include "ai_kiyosumi.h"



namespace saki
{



Action AiNodoka::think(const TableView &view, Limits &limits)
{
    return Ai::thinkChoices(view, limits);
}



} // namespace saki
