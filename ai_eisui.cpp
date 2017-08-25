#include "ai_eisui.h"



namespace saki
{



Action AiHatsumi::think(const TableView &view, Limits &limits)
{
    if (view.getSelfWind(mSelf) == 4) {
        using namespace tiles34;

        switch  (view.myChoices().mode()) {
        case Choices::Mode::DRAWN:
            limits.addNoOut(1_f);
            limits.addNoOut(2_f);
            limits.addNoOut(3_f);
            limits.addNoOut(4_f);
            break;
        case Choices::Mode::BARK:
            if (view.myChoices().can(ActCode::DAIMINKAN)) {
                const T37 &pick = view.getFocusTile();
                if (pick == 1_f || pick == 4_f)
                    return Action(ActCode::DAIMINKAN);
            } else if (view.myChoices().can(ActCode::PON)) {
                const T37 &pick = view.getFocusTile();
                if (pick == 1_f || pick == 4_f) {
                    auto list = listCp(view.myHand(), view.myChoices().bark(), pick);
                    return Ai::thinkAttackStep(view, list);
                }
            }
            break;
        default:
            break;
        }
    }

    return Ai::think(view, limits);
}



Action AiKasumi::forward(const TableView &view)
{
    if (view.myChoices().can(ActCode::IRS_CHECK)) {
        int remRound = view.getRuleInfo().roundLimit - view.getRound();
        unsigned mask = remRound < 5 && view.myRank() != 1 ? 0b1 : 0b0;
        return Action(ActCode::IRS_CHECK, mask);
    } else {
        return Action();
    }
}



} // namespace saki


