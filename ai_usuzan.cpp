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

int AiSawaya::happy(const TableView &view, int iter, const Action &action)
{
    const Sawaya &girl = static_cast<const Sawaya&>(view.me());
    assert(girl.getId() == Girl::Id::SHISHIHARA_SAWAYA);

    if (!girl.usingRedCloud() || view.getRiver(mSelf).size() > 3)
        return Ai::happy(view, iter, action);

    if (iter == 0) {
        if (action.isDiscard()) {
            // reserve Z when using red cloud
            const T37 &tile = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                                : view.myHand().drawn();
            return tile.isZ() ? 0 : 1;
        } else {
            return 1;
        }
    } else {
        return Ai::happy(view, iter - 1, action);
    }
}



} // namespace saki


