#include "ai_shiraitodai_awai.h"



namespace saki
{



Action AiAwai::thinkIrs(const TableView &view)
{
    assert(view.myChoices().can(ActCode::IRS_CHECK));

    unsigned mask = view.myRank() == 1 ? 0b0 : 0b1;
    return Action(ActCode::IRS_CHECK, mask);
}



} // namespace saki
