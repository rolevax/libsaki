#ifndef SAKI_APP_GIRL_X_H
#define SAKI_APP_GIRL_X_H

#include "../table/girl.h"
#include "../3rd/kaguya.hpp"

#include <sstream>


namespace saki
{


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
    void setupLuaClasses(kaguya::LuaTable girl);
    void setupLuaWho(kaguya::LuaTable girl);
    void setupLuaTile(kaguya::LuaTable girl);
    void setupLuaMount(kaguya::LuaTable girl);
    void setupLuaTileCount(kaguya::LuaTable girl);
    void setupLuaHand(kaguya::LuaTable girl);
    void setupLuaGame(kaguya::LuaTable girl);
    void addError(const char *what);

    void popUpIfAny(const Table &table);

private:
    kaguya::State mLua;
    std::ostringstream mErrStream;
};



} // namespace saki



#endif // SAKI_APP_GIRL_X_H
