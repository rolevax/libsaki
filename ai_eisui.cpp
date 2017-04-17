#include "ai_eisui.h"



namespace saki
{



Action AiHatsumi::think(const TableView &view, const std::vector<Action> &choices)
{
    // pick dmk-F
    auto take = [&view](const Action &action) {
        return action.act() == ActCode::DAIMINKAN
                && view.getFocusTile().suit() == Suit::F;
    };

    auto it = std::find_if(choices.begin(), choices.end(), take);
    if (it != choices.end())
        return *it;

    // filter-out discard-F
    auto pass = [&view](const Action &action) {
        if (action.isDiscard()) {
            const T37 &tile = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                                : view.myHand().drawn();
            return tile.suit() != Suit::F;
        } else {
            return true;
        }
    };

    return Ai::think(view, filter(choices, pass));
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


