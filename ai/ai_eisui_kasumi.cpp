#include "ai_eisui_kasumi.h"



namespace saki
{



Action AiKasumi::thinkIrs(const TableView &view)
{
    assert(view.myChoices().can(ActCode::IRS_CHECK));

    int remRound = view.getRule().roundLimit - view.getRound();
    unsigned mask = remRound < 5 && view.myRank() != 1 ? 0b1 : 0b0;
    return Action(ActCode::IRS_CHECK, mask);
}



} // namespace saki
