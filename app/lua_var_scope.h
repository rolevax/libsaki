#ifndef SAKI_APP_LUA_VAR_SCOPE_H
#define SAKI_APP_LUA_VAR_SCOPE_H

#include "../3rd/sol.hpp"

#include <functional>


namespace saki
{



class LuaVarScope
{
public:
    template<typename... Args>
    explicit LuaVarScope(sol::environment girl, Args... args)
    {
        auto seq = std::make_index_sequence<sizeof...(args) / 2>();
        auto tuple = std::make_tuple(args...);
        setup(girl, seq, tuple);
        mDtor = [=]() {
            clear(girl, seq, tuple);
        };
    }

    ~LuaVarScope()
    {
        mDtor();
    }

    template<size_t... Is, typename Tuple>
    void setup(sol::environment girl, std::index_sequence<Is...>, Tuple tuple)
    {
        (girl.raw_set(std::get<2 * Is>(tuple), std::get<2 * Is + 1>(tuple)), ...);
    }

    template<size_t... Is, typename Tuple>
    void clear(sol::environment girl, std::index_sequence<Is...>, Tuple tuple)
    {
        (girl.raw_set(std::get<2 * Is>(tuple), nullptr), ...);
    }

private:
    std::function<void()> mDtor;
};




} // namespace saki



#endif // SAKI_APP_LUA_VAR_SCOPE_H
