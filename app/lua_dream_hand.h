#ifndef SAKI_APP_LUA_DREAM_HAND_H
#define SAKI_APP_LUA_DREAM_HAND_H

#include "lua_class.h"
#include "../form/hand.h"



namespace saki
{



class LuaDreamHand : public Hand
{
public:
    explicit LuaDreamHand(const Hand &hand, LuaUserErrorHandler &error);

    void safeDraw(const T37 &in);
    void safeSwapOut(const T37 &out);
    void safeSpinOut();
    // TODO
//    void barkOut(const T37 &out);
//    void chiiAsLeft(const T37 &pick, bool showAka5);
//    void chiiAsMiddle(const T37 &pick, bool showAka5);
//    void chiiAsRight(const T37 &pick, bool showAka5);
//    void pon(const T37 &pick, int showAka5, int layIndex);
//    void daiminkan(const T37 &pick, int layIndex);
//    void ankan(T34 t);
//    void kakan(int barkId);

private:
    LuaUserErrorHandler &mError;
};


} // namespace saki



#endif // SAKI_APP_LUA_DREAM_HAND_H
