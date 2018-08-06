#ifndef SAKI_APP_LUA_CLASS_H
#define SAKI_APP_LUA_CLASS_H

#include "../3rd/sol.hpp"


namespace saki
{



class LuaUserErrorHandler
{
public:
    virtual void handleUserError(const char *msg) = 0;
};



void setupLuaClasses(sol::environment env, LuaUserErrorHandler &error);
void setupLuaTile(sol::environment env, LuaUserErrorHandler &error);
void setupLuaWho(sol::environment env);
void setupLuaMeld(sol::environment env);
void setupLuaMount(sol::environment env);
void setupLuaTileCount(sol::environment env);
void setupLuaHand(sol::environment env);
void setupLuaGame(sol::environment env);



} // namespace saki



#endif // SAKI_APP_LUA_CLASS_H
