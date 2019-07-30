#ifndef SAKI_APP_LUA_CLASS_H
#define SAKI_APP_LUA_CLASS_H

#include "lua_user_error_handler.h"
#include "../3rd/sol.hpp"
#include "../table/table_event.h"


namespace saki
{



void setupLuaClasses(const sol::environment &env, LuaUserErrorHandler &error);
void setupLuaRand(sol::environment env);
void setupLuaTile(sol::environment env, LuaUserErrorHandler &error);
void setupLuaWho(sol::environment env);
void setupLuaMeld(sol::environment env, LuaUserErrorHandler &error);
void setupLuaExist(sol::environment env, LuaUserErrorHandler &error);
void setupLuaMount(sol::environment env);
void setupLuaTileCount(sol::environment env, LuaUserErrorHandler &error);
void setupLuaHand(sol::environment env);
void setupLuaDreamHand(sol::environment env, LuaUserErrorHandler &error);
void setupLuaForm(sol::environment env, LuaUserErrorHandler &error);
void setupLuaRule(sol::environment env);
void setupLuaFormCtx(sol::environment env);
void setupLuaGame(sol::environment env);

sol::table toLuaTable(sol::environment env, const TableEvent &event);


} // namespace saki



#endif // SAKI_APP_LUA_CLASS_H
