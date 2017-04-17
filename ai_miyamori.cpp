#include "ai_miyamori.h"
#include "util.h"



namespace saki
{



Action AiToyone::think(const TableView &view, const std::vector<Action> &choices)
{
    // always riichi when available
    auto take = [](const Action &act) { return act.act() == ActCode::RIICHI; };
    auto it = std::find_if(choices.begin(), choices.end(), take);
    if (it != choices.end())
        return *it;

    auto pass = [](const Action &action) {
        // as tomibiki is not working so well
        // also, override 1pt-canceling in defense
        return !action.isCpdmk();
    };

    return Ai::think(view, filter(choices, pass));
}



} // namespace saki


