#include "ai_shiraitodai.h"
#include "girls_shiraitodai.h"
#include "util.h"



namespace saki
{



int AiTakami::happy(const TableView &view, int iter, const Action &action)
{
    if (view.getRiver(mSelf).empty()
            && view.myHand().barks().empty()
            && !view.isAllLast()) {
        if (iter > 0)
            return Ai::happy(view, iter - 1, action);

        if (action.isDiscard()) {
            // first discard of a non-all-last
            const Takami &takami = static_cast<const Takami&>(view.me());
            assert(takami.getId() == Girl::Id::SHIBUYA_TAKAMI);
            const T37 &out = action.act() == ActCode::SWAP_OUT ? action.tile()
                                                               : view.myHand().drawn();
            return out.isZ() + (out.suit() == Suit::Y) + takami.d3gNeed(out);
        } else {
            return action.act() == ActCode::TSUMO ? 6 : 0;
        }
    } else {
        return Ai::happy(view, iter, action);
    }
}



int AiSeiko::happy(const TableView &view, int iter, const Action &action)
{
    if (iter > 0)
        return Ai::happy(view, iter - 1, action);

    if (action.isCpdmk()) {
        if (view.myHand().barks().size() < 3) {
            ActCode act = action.act();
            switch (act) {
            case ActCode::DAIMINKAN:
                return 3;
            case ActCode::PON:
                return 2;
            default: // chii cases
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        return 1;
    }
}



Action AiAwai::forward(const TableView &view)
{
    if (view.iCan(ActCode::IRS_CHECK)) {
        unsigned mask = view.myRank() == 1 ? 0b0 : 0b1;
        return Action(ActCode::IRS_CHECK, mask);
    } else {
        return Action();
    }
}



} // namespace saki


