#include "girl_x.h"
#include "lua_var_scope.h"
#include "../table/table.h"



namespace saki
{



GirlX::GirlX(Who who, std::string luaCode)
    : Girl(who, Girl::Id::CUSTOM)
{
    setupLuaGlobal();

    runInGirlEnv(luaCode);
}

GirlX::GirlX(const GirlX &copy)
    : Girl(copy)
{
    // TODO
    // support cloning Lua state to enable skills inside future vision
    // strategy:
    // - Copy everything reachable from the 'girldata' table
    // - Ignore metatables
    //     - Userdata metatables are generated by fixed steps
    // - Ignore coroutines
    // - Deep copy all GC object
    // - Smartize Objects whose lifetime managed by C++
    // - Copy C functions (i.e. pointers), deep copy their upvalues
    // - Copy Lua functions by lua_dump, deep copy their upvalues
    // - Type-switch userdatas, use C++ copy ctor to copy them
    // deep copy should detect cycle

//    setupLuaGlobal();
//    cloneLuaTable(mLua["girldata"], const_cast<GirlX &>(copy).mLua["girldata"]);
}

std::unique_ptr<Girl> GirlX::clone() const
{
    return std::make_unique<GirlX>(*this);
}

bool GirlX::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    sol::object cb = mGirlEnv["checkinit"];
    if (!cb.is<sol::function>())
        return true;

    if (iter > 1000)
        return true;

    LuaVarScope scope(
        mGirlEnv,
        "game", &table,
        "init", &init,
        "who", who,
        "iter", iter
    );

    (void) scope;

    auto res = runInGirlEnv("return checkinit()");
    if (!res.is<bool>())
        addError("non-boolean checkinit() result");

    popUpIfAny(table);
    return res.is<bool>() ? res.as<bool>() : true;
}

void GirlX::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    sol::object cb = mGirlEnv["ondraw"];
    if (!cb.is<sol::function>())
        return;

    LuaVarScope scope(
        mGirlEnv,
        "game", &table,
        "mount", &mount,
        "who", who,
        "rinshan", rinshan
    );

    (void) scope;

    runInGirlEnv("ondraw()");
    popUpIfAny(table);
}

std::string GirlX::popUpStr() const
{
    std::ostringstream oss;
    oss << "Lua";
    oss << mErrStream.str();
    return oss.str();
}

void GirlX::onTableEvent(const Table &table, const TableEvent &event)
{
    if (event.type() == TableEvent::Type::POPPED_UP)
        return; // skills shouldn't peek private outputs

    if (event.type() == TableEvent::Type::TABLE_STARTED)
        popUpIfAny(table);

    sol::object cb = mGirlEnv["ontableevent"];
    if (!cb.is<sol::function>())
        return;

    LuaVarScope scope(
        mGirlEnv,
        "game", &table,
        "event", toLuaTable(mGirlEnv, event)
    );

    (void) scope;

    runInGirlEnv("ontableevent()");
    popUpIfAny(table);
}

void GirlX::setupLuaGlobal()
{
    mLua.open_libraries();

    mLua.script(R"(
        girldata = {}
        girl = {
            math = math,
            table = table,
            string = string,
            pairs = pairs,
            ipairs = ipairs,
            print = function(...)
                local args = table.pack(...)
                for i = 1, args.n do
                    girl.printone(args[i])
                    girl.printone(" ")
                end
                girl.printone("\n")
            end,
            __index = function(t, k)
                return girldata[k]
            end,
            __newindex = function(t, k, v)
                girldata[k] = v
            end
        }
        girl.math.random = nil
        girl.math.randomseed = nil
    )");

    mGirlEnv = mLua["girl"];

    setupLuaClasses(mGirlEnv, *this);
    mGirlEnv["self"] = mSelf;
    mGirlEnv["printone"] = [this](sol::reference ref) {
        std::string str = mLua["tostring"](ref);
        mErrStream << str;
    };

    mGirlEnv[sol::metatable_key] = mGirlEnv;
}

void GirlX::addError(const char *what)
{
    mErrStream << what << '\n';
}

sol::object GirlX::runInGirlEnv(const std::string_view &code) noexcept
{
    sol::object obj;

    auto res = mLua.safe_script(code, mGirlEnv, sol::script_pass_on_error);
    if (!res.valid()) {
        sol::error e = res;
        addError(e.what());
    } else {
        obj = res;
    }

    return obj;
}

void GirlX::popUpIfAny(const Table &table)
{
    if (mErrStream.tellp() > 0) {
        table.popUp(mSelf);
        mErrStream.str("");
    }
}

void GirlX::handleUserError(const char *msg)
{
    mLua["error"](msg);
}



} // namespace saki
