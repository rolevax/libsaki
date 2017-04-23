#include "ai_shiraitodai.h"
#include "girls_shiraitodai.h"
#include "util.h"



namespace saki
{



Action AiTakami::think(const TableView &view, const std::vector<Action> &choices)
{
    Action act = thinkTrivial(choices);
    if (act.act() != ActCode::NOTHING)
        return act;

    if (view.getRiver(mSelf).empty()
            && view.myHand().barks().empty()
            && !view.isAllLast()) {
        // first discard of a non-all-last
        const Takami &takami = static_cast<const Takami&>(view.me());
        assert(takami.getId() == Girl::Id::SHIBUYA_TAKAMI);

        int max = 0;
        Action res;
        for (const Action &act : choices) {
            if (act.isDiscard()) {
                const T37 &out = act.act() == ActCode::SWAP_OUT ? act.tile()
                                                                : view.myHand().drawn();
                int comax = out.isZ() + (out.suit() == Suit::Y) + takami.d3gNeed(out);
                if (comax > max) {
                    max = comax;
                    res = act;
                }
            }
        }

        if (res.act() != ActCode::NOTHING)
            return res;
    }

    return Ai::think(view, choices);
}



Action AiSeiko::think(const TableView &view, const std::vector<Action> &choices)
{
    auto find = [&choices](ActCode act) {
        auto is = [act](const Action &a) { return a.act() == act; };
        return std::find_if(choices.begin(), choices.end(), is);
    };

    auto it = find(ActCode::RON);
    if (it != choices.end())
        return *it;

    it = find(ActCode::DAIMINKAN);
    if (it != choices.end())
        return *it;

    it = find(ActCode::PON);
    if (it != choices.end())
        return *it;

    // never chii
    auto pass = [](const Action &a) { return !a.isChii(); };
    return Ai::think(view, filter(choices, pass));
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


