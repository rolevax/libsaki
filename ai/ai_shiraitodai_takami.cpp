#include "ai_shiraitodai_takami.h"
#include "../girl/shiraitodai_takami.h"



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



} // namespace saki
