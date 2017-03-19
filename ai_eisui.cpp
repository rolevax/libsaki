#include "ai_eisui.h"



namespace saki
{



int AiHatsumi::happy(const TableView &view, int iter, const Action &action)
{
    if (iter > 0)
        return Ai::happy(view, iter - 1, action);

    // pick dmk-F, filter-out discard-F
    if (action.act() == ActCode::DAIMINKAN && view.getFocusTile().suit() == Suit::F) {
        return 2;
    } else if (action.isDiscard()) {
        const T37 &tile = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                            : view.myHand().drawn();
        return tile.suit() == Suit::F ? 0 : 1;
    } else {
        return 1;
    }
}



Action AiKasumi::forward(const TableView &view)
{
    if (view.iCan(ActCode::IRS_CHECK)) {
        int remRound = view.getRuleInfo().roundLimit - view.getRound();
        unsigned mask = remRound < 5 && view.myRank() != 1 ? 0b1 : 0b0;
        return Action(ActCode::IRS_CHECK, mask);
    } else {
        return Action();
    }
}



} // namespace saki


