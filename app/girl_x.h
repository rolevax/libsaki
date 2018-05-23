#ifndef SAKI_APP_GIRL_X_H
#define SAKI_APP_GIRL_X_H

#include "../table/girl.h"
#include "../3rd/sol.hpp"

#include <sstream>


namespace saki
{



///
/// \brief Lua intepreter for custom characters
///
class GirlX : public Girl
{
public:
    GirlX(Who who, std::string luaCode);
    GirlX(const GirlX &copy);

    std::unique_ptr<Girl> clone() const override;

    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    std::string popUpStr() const override;

    void onTableEvent(const Table &table, const TableEvent &event) override;

private:
    void setupLuaGlobal();
    void setupLuaClasses(sol::table girl);
    void setupLuaTile(sol::table girl);
    void setupLuaWho(sol::table girl);
    void setupLuaMount(sol::table girl);
    void setupLuaTileCount(sol::table girl);
    void setupLuaHand(sol::table girl);
    void setupLuaGame(sol::table girl);
    void addError(const char *what);

    void runInGirlEnv(const std::string_view &code);

    void popUpIfAny(const Table &table);

private:
    sol::state mLua;
    std::ostringstream mErrStream;
};



} // namespace saki



#endif // SAKI_APP_GIRL_X_H
