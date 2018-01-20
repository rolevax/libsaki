#include "ai_shiraitodai.h"
#include "../girl/girls_shiraitodai.h"



namespace saki
{



Action AiTakami::think(const TableView &view, Limits &limits)
{
    // first discard of a non-all-last
    if (!view.isAllLast()
        && view.getRiver(view.self()).empty()
        && view.myChoices().mode() == Choices::Mode::DRAWN) {
        const Takami &takami = static_cast<const Takami &>(view.me());
        assert(takami.getId() == Girl::Id::SHIBUYA_TAKAMI);

        const Hand &hand = view.myHand();

        int max = 0;
        Action res;

        // *INDENT-OFF*
        auto update = [&takami, &hand, &max, &res](const Action &action) {
            const T37 &out = hand.outFor(action);
            int comax = out.isZ() + (out.suit() == Suit::Y) + takami.d3gNeed(out);
            if (comax > max) {
                max = comax;
                res = action;
            }
        };
        // *INDENT-ON*

        update(Action(ActCode::SPIN_OUT));
        for (const T37 &out : hand.closed().t37s13())
            update(Action(ActCode::SWAP_OUT, out));

        if (res.act() == ActCode::NOTHING)
            res = Ai::think(view, limits);

        return res;
    }

    return Ai::think(view, limits);
}



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



Action AiAwai::forward(const TableView &view)
{
    if (view.myChoices().can(ActCode::IRS_CHECK)) {
        unsigned mask = view.myRank() == 1 ? 0b0 : 0b1;
        return Action(ActCode::IRS_CHECK, mask);
    } else {
        return Action();
    }
}



} // namespace saki
