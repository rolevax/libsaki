#ifndef SAKI_APP_GIRL_X_H
#define SAKI_APP_GIRL_X_H

#include "lua_class.h"
#include "../table/girl.h"

#include <sstream>


namespace saki
{



///
/// \brief Lua intepreter for custom characters
///
class GirlX : public Girl, public LuaUserErrorHandler
{
public:
    GirlX(Who who, std::string luaCode);
    GirlX(const GirlX &copy);

    std::unique_ptr<Girl> clone() const override;

    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    std::string popUpStr() const override;

    void onTableEvent(const Table &table, const TableEvent &event) override;

private:
    void setupLuaGlobal();
    void addError(const char *what);

    sol::object runInGirlEnv(const std::string_view &code);

    void popUpIfAny(const Table &table);

    void handleUserError(const char *msg) override;

private:
    sol::state mLua;
    sol::environment mGirlEnv;
    std::ostringstream mErrStream;
};



} // namespace saki



#endif // SAKI_APP_GIRL_X_H
