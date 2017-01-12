#include "ai_eisui.h"



namespace saki
{



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


