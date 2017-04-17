#include "ai_achiga.h"
#include "util.h"



namespace saki
{



Action AiKuro::think(const TableView &view, const std::vector<Action> &choices)
{
    // menzen + dama + no-ankan
    // --> filter-out cpdmk, ankan, riichi, discard-dora
    auto pass = [&view](const Action &action) {
        if (action.isCpdmk()
                || action.act() == ActCode::ANKAN
                || action.act() == ActCode::RIICHI) {
            return false;
        } else if (action.isDiscard()) {
            const T37 &tile = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                                : view.myHand().drawn();
            return view.getDrids() % tile == 0 && !tile.isAka5();
        } else {
            return true;
        }
    };

    return Ai::think(view, filter(choices, pass));
}



} // namespace saki


