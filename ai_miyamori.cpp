#include "ai_miyamori.h"
#include "util.h"



namespace saki
{



int AiToyone::happy(const TableView &view, int iter, const Action &action)
{
    if (iter > 0)
        return Ai::happy(view, iter - 1, action);

    if (action.isCpdmk()) {
        // as tomibiki is not working so well in 0.7.4
        // also, override 1pt-canceling in defense
        return 0;
    } else if (action.act() == ActCode::RIICHI) {
        return 2; // afraid what afraid
    } else {
        return 1;
    }
}



} // namespace saki


