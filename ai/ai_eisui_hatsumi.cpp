#include "ai_eisui_hatsumi.h"



namespace saki
{



Action AiHatsumi::think(const TableView &view, Limits &limits)
{
    if (view.getSelfWind(view.self()) == 4) {
        using namespace tiles34;

        switch (view.myChoices().mode()) {
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
                    auto list = listCp(view.myHand(), view.myChoices().bark(), pick, true);
                    return Ai::thinkAttackStep(view, list.range());
                }
            }

            break;
        default:
            break;
        }
    }

    return Ai::think(view, limits);
}



} // namespace saki
