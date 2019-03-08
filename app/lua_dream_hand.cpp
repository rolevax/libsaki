#include "lua_dream_hand.h"



namespace saki
{



LuaDreamHand::LuaDreamHand(const Hand &hand, LuaUserErrorHandler &error)
    : Hand(hand)
    , mError(error)
{
}

void LuaDreamHand::safeDraw(const T37 &in)
{
    if (hasDrawn()) {
        mError.handleUserError("EDrmCntDrw");
        return;
    }

    draw(in);
}

void LuaDreamHand::safeSwapOut(const T37 &out)
{
    if (!(closed().ct(out) > 0) && hasDrawn()) {
        mError.handleUserError("EDrmCntSwp");
        return;
    }

    swapOut(out);
}

void LuaDreamHand::safeSpinOut()
{
    if (!hasDrawn()) {
        mError.handleUserError("EDrmCntSwp");
        return;
    }

    spinOut();
}



} // namespace saki
