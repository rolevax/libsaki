#ifndef SAKI_APP_LUA_VAR_SCOPE_H
#define SAKI_APP_LUA_VAR_SCOPE_H

#include "../3rd/sol.hpp"

#include <functional>


namespace saki
{



template<typename... Args>
class LuaVarScope
{
public:
    explicit LuaVarScope(sol::environment girl, Args ... args)
        : mGirl(girl)
        , mArgs(std::make_tuple(std::move(args) ...))
    {
        setup(std::make_index_sequence<sizeof...(args) / 2>());
    }

    ~LuaVarScope()
    {
        clear(std::make_index_sequence<std::tuple_size<decltype(mArgs)>() / 2>());
    }

    template<size_t... Is>
    void setup(std::index_sequence<Is...>)
    {
        // safe to move the lvalue tuple, because the moved value won't be used anymore
        (mGirl.raw_set(std::get<2 * Is>(mArgs), std::get<2 * Is + 1>(std::move(mArgs))), ...);
    }

    template<size_t... Is>
    void clear(std::index_sequence<Is...>)
    {
        (mGirl.raw_set(std::get<2 * Is>(mArgs), nullptr), ...);
    }

private:
    sol::environment mGirl;
    std::tuple<Args ...> mArgs;
};




} // namespace saki



#endif // SAKI_APP_LUA_VAR_SCOPE_H
