#include "ai_achiga.h"
#include "util.h"



namespace saki
{



int AiKuro::happy(const TableView &view, int iter, const Action &action)
{
    if (iter > 0)
        return Ai::happy(view, iter - 1, action);

    // menzen + dama + no-ankan
    // --> filter-out cpdmk, ankan, riichi, discard-dora
    if (action.isCpdmk()
            || action.act() == ActCode::ANKAN
            || action.act() == ActCode::RIICHI) {
        return 0;
    } else if (action.isDiscard()) {
        const T37 &tile = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                            : view.myHand().drawn();
        return view.getDrids() % tile == 0 && !tile.isAka5() ? 1 : 0;
    } else {
        return 1;
    }
}



} // namespace saki


