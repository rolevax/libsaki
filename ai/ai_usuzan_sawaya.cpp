#include "ai_usuzan_sawaya.h"
#include "../girl/usuzan_sawaya.h"

#include <algorithm>



namespace saki
{



Action AiSawaya::thinkIrs(const TableView &view)
{
    if (view.myChoices().can(ActCode::IRS_CHECK)) {
        unsigned mask = 0;
        if (view.getDealer() == view.self()) {
            const Sawaya &girl = static_cast<const Sawaya &>(view.me());
            assert(girl.getId() == Girl::Id::SHISHIHARA_SAWAYA);

            // confusing code. change it someday...
            girl.canUseRedCloud(mask)
            || girl.canUseWhiteCloud(mask);
        }

        return Action(ActCode::IRS_CHECK, mask);
    } else {
        Limits limits;
        return think(view, limits);
    }
}

Action AiSawaya::think(const TableView &view, Limits &limits)
{
    const Sawaya &girl = static_cast<const Sawaya &>(view.me());
    assert(girl.getId() == Girl::Id::SHISHIHARA_SAWAYA);

    if (!girl.usingRedCloud() || view.getRiver(view.self()).size() > 3)
        return Ai::think(view, limits);

    // reserve Z when using red cloud
    for (T34 z : tiles34::Z7)
        limits.addNoOut(z);

    return Ai::think(view, limits);
}



} // namespace saki
