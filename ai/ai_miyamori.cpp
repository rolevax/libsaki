#include "ai_miyamori.h"



namespace saki
{



Action AiToyone::think(const TableView &view, Limits &limits)
{
    // as tomibiki is not working so well (until v0.8.3)
    // also, override 1pt-canceling in defense
    limits.addNoBark();

    // always riichi whenever available
    if (view.myChoices().canRiichi()) {
        Action riichi;
        Ai::testRiichi(view, limits, riichi);
        assert(riichi.isRiichi());
        return riichi;
    }

    return Ai::think(view, limits);
}



} // namespace saki
