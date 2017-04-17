#include "ai_usuzan.h"
#include "girls_usuzan.h"

#include <algorithm>
#include <cassert>



namespace saki
{



Action AiSawaya::forward(const TableView &view)
{
    if (view.iCan(ActCode::IRS_CHECK)) {
        unsigned mask = 0;
        if (view.getDealer() == mSelf) {
            const Sawaya &girl = static_cast<const Sawaya&>(view.me());
            assert(girl.getId() == Girl::Id::SHISHIHARA_SAWAYA);

            // confusing code. change it someday...
            girl.canUseRedCloud(mask)
                    || girl.canUseWhiteCloud(mask);
        }
        return Action(ActCode::IRS_CHECK, mask);
    } else {
        return Action();
    }
}

Action AiSawaya::think(const TableView &view, const std::vector<Action> &choices)
{
    const Sawaya &girl = static_cast<const Sawaya&>(view.me());
    assert(girl.getId() == Girl::Id::SHISHIHARA_SAWAYA);

    if (!girl.usingRedCloud() || view.getRiver(mSelf).size() > 3)
        return Ai::think(view, choices);

    // reserve Z when using red cloud
    auto pass = [&view](const Action &act) {
        if (act.isDiscard()) {
            const T37 &tile = act.act() == ActCode::SWAP_OUT ? act.tile()
                                                             : view.myHand().drawn();
            return !tile.isZ();
        } else {
            return true;
        }
    };

    return Ai::think(view, filter(choices, pass));
}



} // namespace saki


