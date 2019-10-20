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

    LuaManagedRef(LuaManagedRef &&that) noexcept
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

    auto data() const -> const T *
    {
        return mData;
    }

    auto error() const -> LuaUserErrorHandler &
    {
        return mError;
    }

    template<typename Ret, typename ... Args>
    static auto makeConstMethod(Ret (T::*method)(Args...) const) -> std::function<Ret(LuaManagedRef *, Args...)>
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(false))
                return Ret();

            return (thiz->mData->*method)(args...);
        };
    }

    template<typename Ret, typename ... Args>
    static auto makeMutableMethod(Ret (T::*method)(Args...)) -> std::function<Ret(LuaManagedRef *, Args...)>
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
    static auto makeConstMethodAsConstRef(const Ret &(T::*method)(Args...) const)
        -> std::function<std::unique_ptr<LuaManagedRef<Ret>>(LuaManagedRef *, Args...)>
    {
        return [method](LuaManagedRef *thiz, Args ... args) -> std::unique_ptr<LuaManagedRef<Ret>> {
            if (thiz->isBadReference(false))
                return nullptr;

            const Ret &r = (thiz->mData->*method)(args...);
            return std::make_unique<LuaManagedRef<Ret>>(&r, thiz->mError, false);
        };
    }

    template<typename Ret, typename ... Args>
    static auto makeConstFunction(Ret (*method)(const T &, Args...))
        -> std::function<Ret(LuaManagedRef *, Args...)>
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(false))
                return Ret();

            return method(*thiz->mData, args...);
        };
    }

    template<typename Ret, typename ... Args>
    static auto makeMutableFunction(Ret (*method)(T &, Args...))
        -> std::function<Ret(LuaManagedRef *, Args...)>
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(true))
                return Ret();

            return method(*thiz->mData, args...);
        };
    }

    template<typename Ret, typename ... Args>
    static auto makeConstFunctionError(Ret (*method)(LuaUserErrorHandler &, const T &, Args...))
        -> std::function<Ret(LuaManagedRef *, Args...)>
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(false))
                return Ret();

            return method(thiz->mError, *thiz->mData, args...);
        };
    }

    template<typename Ret, typename ... Args>
    static auto makeMutableFunctionError(Ret (*method)(LuaUserErrorHandler &, T &, Args...))
        -> std::function<Ret(LuaManagedRef *, Args...)>
    {
        return [method](LuaManagedRef *thiz, Args ... args) {
            if (thiz->isBadReference(true))
                return Ret();

            return method(thiz->mError, *thiz->mData, args...);
        };
    }

private:
    auto isBadReference(bool modifying) -> bool
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
