#include "ai_senriyama.h"



namespace saki
{



Action AiToki::thinkIrs(const TableView &view)
{
    (void) view;
    Limits limits;
    return think(view, limits);
}



} // namespace saki
