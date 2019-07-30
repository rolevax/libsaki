#ifndef SAKI_APP_LUA_MANAGED_REF_H
#define SAKI_APP_LUA_MANAGED_REF_H

#include "lua_user_error_handler.h"
#include "../3rd/sol.hpp"
#include "../util/misc.h"

#include <functional>
#include <tuple>


namespace saki
{



template <typename T>
class LuaManagedRef
{
public:
    LuaManagedRef(T *data, LuaUserErrorHandler &error, bool isOwning, bool isConst)
        : mData(data)
        , mError(error)
        , mIsOwning(isOwning)
        , mIsConst(isConst)
    {
    }

    LuaManagedRef(const T *data, LuaUserErrorHandler &error, bool isOwning)
        : LuaManagedRef(const_cast<T *>(data), error, isOwning, true)
    {
    }

    ~LuaManagedRef()
    {
        if (mIsOwning)
            delete mData;
    }

    LuaManagedRef(LuaManagedRef &&that)
        : mData(that.mData)
        , mError(that.mError)
        , mIsOwning(that.mIsOwning)
        , mIsConst(that.mIsConst)
    {
        that.mData = nullptr;
    }

    void dispose()
    {
        mData = nullptr;
    }

    template<typename Ret, typename ... Args>
    static std::function<Ret(LuaManagedRef *, Args...)> makeConstMethod(Ret (T::*method)(Args...) const)
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(false))
                return Ret();

            return (thiz->mData->*method)(args...);
        };
    }

    template<typename Ret, typename ... Args>
    static std::function<Ret(LuaManagedRef *, Args...)> makeMutableMethod(Ret (T::*method)(Args...))
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(true))
                return Ret();

            return (thiz->mData->*method)(args...);
        };
    }

    template<typename Ret, typename ... Args>
    static auto makeConstMethodAsTable(Ret (T::*method)(Args...) const)
        -> std::function<sol::as_table_t<sol::meta::unqualified_t<Ret>>(LuaManagedRef *, Args...)>
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(false))
                return sol::as_table_t<sol::meta::unqualified_t<Ret>>();

            return sol::as_table((thiz->mData->*method)(args...));
        };
    }

    template<typename Ret, typename ... Args>
    static auto makeMutableMethodAsTable(Ret (T::*method)(Args...))
        -> std::function<sol::as_table_t<sol::meta::unqualified_t<Ret>>(LuaManagedRef *, Args...)>
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(true))
                return sol::as_table_t<sol::meta::unqualified_t<Ret>>();

            return sol::as_table((thiz->mData->*method)(args...));
        };
    }

    template<typename Ret, typename ... Args>
    static std::function<Ret(LuaManagedRef *, Args...)> makeConstFunction(Ret (*method)(const T &, Args...))
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(false))
                return Ret();

            return method(*thiz->mData, args...);
        };
    }

    template<typename Ret, typename ... Args>
    static std::function<Ret(LuaManagedRef *, Args...)> makeMutableFunction(Ret (*method)(T &, Args...))
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(true))
                return Ret();

            return method(*thiz->mData, args...);
        };
    }

    template<typename Ret, typename ... Args>
    static std::function<Ret(LuaManagedRef *, Args...)> makeConstFunctionError(Ret (*method)(LuaUserErrorHandler &, const T &, Args...))
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(false))
                return Ret();

            return method(thiz->mError, *thiz->mData, args...);
        };
    }

    template<typename Ret, typename ... Args>
    static std::function<Ret(LuaManagedRef *, Args...)> makeMutableFunctionError(Ret (*method)(LuaUserErrorHandler &, T &, Args...))
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(true))
                return Ret();

            return method(thiz->mError, *thiz->mData, args...);
        };
    }

private:
    bool isBadReference(bool modifying)
    {
        if (mData == nullptr) {
            mError.handleUserError("ERefNil");
            return true;
        }

        if (modifying && mIsConst) {
            mError.handleUserError("ERefCon");
            return true;
        }

        return false;
    }

private:
    T *mData = nullptr;
    LuaUserErrorHandler &mError;
    bool mIsOwning = false;
    bool mIsConst = false;
};

template<typename ...Ts>
class LuaDisposeRefGuard
{
public:
    LuaDisposeRefGuard(std::unique_ptr<LuaManagedRef<Ts>> & ... pointers)
    {
        mPointers = std::make_tuple(pointers.get() ...);
    }

    ~LuaDisposeRefGuard()
    {
        std::apply([](auto ...p) { (p->dispose(), ...); }, mPointers);
    }

private:
    std::tuple<LuaManagedRef<Ts> * ...> mPointers;
};



} // namespace saki



#endif // SAKI_APP_LUA_MANAGED_REF_H
