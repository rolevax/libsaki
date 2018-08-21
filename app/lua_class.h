#ifndef SAKI_APP_LUA_CLASS_H
#define SAKI_APP_LUA_CLASS_H

#include "../3rd/sol.hpp"
#include "../table/table_event.h"


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
void setupLuaMount(sol::environment env, LuaUserErrorHandler &error);
void setupLuaTileCount(sol::environment env, LuaUserErrorHandler &error);
void setupLuaHand(sol::environment env);
void setupLuaGame(sol::environment env);

sol::table toLuaTable(sol::environment env, const TableEvent &event);


} // namespace saki



#endif // SAKI_APP_LUA_CLASS_H
