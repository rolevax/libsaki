#include "ai_achiga.h"



namespace saki
{



Action AiKuro::think(const TableView &view, Limits &limits)
{
    // dama, no-ankan
    limits.addNoBark();
    limits.addNoRiichi();
    limits.addNoAnkan();

    // keep dora
    limits.addNoOutAka5();
    for (T34 id : view.getDrids())
        limits.addNoOut(id.dora());

    return Ai::think(view, limits);
}



} // namespace saki
