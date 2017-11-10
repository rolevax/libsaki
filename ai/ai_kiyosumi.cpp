#include "ai_kiyosumi.h"



namespace saki {



Action AiNodoka::think(const TableView &view, Limits &limits)
{
    // not to antiHatsumi & antiToyone
//    antiHatsumi(view, limits);
//    antiToyone(view, limits);

    const Choices &choices = view.myChoices();

    switch (choices.mode()) {
    case Choices::Mode::WATCH:
        unreached("Ai::think: unexpected watch mode");
    case Choices::Mode::CUT:
        unreached("Ai::think: unhandled cut mode");
    case Choices::Mode::DICE:
        return Action(ActCode::DICE);
    case Choices::Mode::DRAWN:
        return thinkDrawn(view, limits);
    case Choices::Mode::BARK:
        return thinkBark(view, limits);
    case Choices::Mode::END:
        return Action(choices.can(ActCode::END_TABLE) ? ActCode::END_TABLE : ActCode::NEXT_ROUND);
    default:
        unreached("Ai::think: illegal mode");
    }
}



} // namespace saki
