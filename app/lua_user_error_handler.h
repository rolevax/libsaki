#ifndef SAKI_APP_LUA_USER_ERROR_HANDLER_H
#define SAKI_APP_LUA_USER_ERROR_HANDLER_H



namespace saki
{



class LuaUserErrorHandler
{
public:
    LuaUserErrorHandler() = default;
    LuaUserErrorHandler(const LuaUserErrorHandler &copy) = default;
    LuaUserErrorHandler(LuaUserErrorHandler &&move) = default;
    LuaUserErrorHandler &operator=(const LuaUserErrorHandler &copy) = default;
    LuaUserErrorHandler &operator=(LuaUserErrorHandler &&move) = default;
    virtual ~LuaUserErrorHandler() = default;

    virtual void handleUserError(const char *msg) = 0;
};



} // namespace saki



#endif // SAKI_APP_LUA_USER_ERROR_HANDLER_H
