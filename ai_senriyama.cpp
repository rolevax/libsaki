#include "ai_senriyama.h"
#include "tilecount.h"



namespace saki
{



Action AiToki::forward(const TableView &view)
{
    (void) view;
    return Action(); // dummy
}

/*
void AiToki::readLog(const AiView &view, const SkillExpr &expr)
{
    const Player &player = view.myPlayer();
    // shouldn't use 'view' to examine riichi
    // because now is still in future vision
    auto drawIt = std::find(expr.begin(), expr.end(), SkillToken::DRAW);
    if (drawIt != expr.end() && player.isMenzen()) {
        TileCount myCount(player.getHand());
        if (player.hasDrawn())
            myCount[player.getDrawn()]++;
        myCount[tileOfToken(*++drawIt)]++;
        if (myCount.step(player.getBarks().size()) == -1) {
            sawIppatsu = true;
            return;
        }
    }

    auto isWin = [](SkillToken t) {
        return t == SkillToken::TSUMO || t == SkillToken::RON;
    };

    SkillExpr::const_iterator winIt = expr.begin();
    while (winIt = std::find_if(winIt, expr.end(), isWin), winIt != expr.end()) {
        if (winIt != expr.end()) {
            winIt++; // skip tsumo/ron token
            winIt++; // skip picked tile
        }

        std::vector<Tile> hand;
        while (winIt != expr.end() && tokenIsTile(*winIt))
            hand.push_back(tileOfToken(*winIt++));
        assert(hand.size() % 3 == 1);
        int barkCt = 4 - hand.size() / 3;
        assert(0 <= barkCt && barkCt <= 4);
        TileCount count(hand);
        assert(count.ready(barkCt));
        for (int ti = 0; ti < 34; ti++) {
            Tile t(ti);
            if (count.hasEffA(t, barkCt))
                safes[t.id34()] = false;
        }
    }
}

bool AiToki::toAttack(const AiView &view, std::vector<Ai::Rival> &readyers)
{
    (void) readyers;

    checkRound(view);

    return true;
}

bool AiToki::toRiichi(const AiView &view)
{
    (void) view;
    return sawIppatsu;
}

AiResult AiToki::thinkAttack(const AiView &view)
{
    const Action &act = view.myAction();
    (void) act; // FUCK
    // OLD-FUCK just foolish AI
//    if (act.can(ActCode::IRS_CLICK)) {
//        return AiResult(ActCode::IRS_CLICK, -1);
//    } else if (act.can(ActCode::IRS_DISCARD)) {
//        return AiResult(ActCode::IRS_DISCARD, discardAttack(view));
//    } else if (act.can(ActCode::PASS)) {
//        // naive temp sosution: just don't bark
//        return AiResult(ActCode::PASS, -1);
//    }

    return Ai::thinkAttack(view);
}

int AiToki::discardAttack(const AiView &view)
{
    std::array<bool, 14> mask = view.myAction().getDiscardMask();
    const Player &player = view.myPlayer();
    const std::vector<Tile> &hand = player.getHand();

    for (int i = 0; i < 13; i++)
        if (mask[i] && !safes[hand[i].id34()])
            mask[i] = false;

    if (mask[13] && player.hasDrawn() && !safes[player.getDrawn().id34()])
        mask[13] = false;

    auto it = std::find(mask.begin(), mask.end(), true);

    if (it != mask.end())
        return Ai::discardAttack(view, mask);
    else // every tile is cannon...
        return Ai::discardAttack(view);
}

void AiToki::checkRound(const AiView &view)
{
    if (round != view.getRound() || extra != view.getExtraRound()) {
        round = view.getRound();
        extra = view.getExtraRound();
        safes.fill(true);
        sawIppatsu = false;
    }
}
*/



} // namespace saki


