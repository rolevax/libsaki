#include "ai_shiraitodai_seiko.h"



namespace saki
{



Action AiSeiko::think(const TableView &view, Limits &limits)
{
    const Choices &choices = view.myChoices();

    if (choices.mode() == Choices::Mode::BARK) {
        using AC = ActCode;
        for (AC ac : { AC::RON, AC::DAIMINKAN })
            if (choices.can(ac))
                return Action(ac);

        if (choices.can(AC::PON)) {
            auto list = listCp(view.myHand(), view.myChoices().bark(),
                               view.getFocusTile(), true);
            return Ai::thinkAttackStep(view, list.range());
        }

        return Action(ActCode::PASS);
    }

    return Ai::think(view, limits);
}



} // namespace saki
