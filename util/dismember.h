#ifndef SAKI_DISMEMBER_H
#define SAKI_DISMEMBER_H

#include <type_traits>
#include <utility>



namespace saki
{



///
/// auto f = PredThis(&Class::c) is same as:
/// auto f = [](const Class &c) { return c.pred(); }
///
template<typename Class>
class PredThis
{
public:
    constexpr explicit PredThis(bool (Class::*predMem)() const) noexcept
        : mPredMem(predMem)
    {
    }

    bool operator()(const Class &thiz)
    {
        return (thiz.*mPredMem)();
    }

private:
    bool (Class::*mPredMem)() const;
};

template<typename Class, typename Ret, typename... Args>
class ReturnCopy
{
public:
    using Method = Ret (Class::*)(Args...) const;

    explicit ReturnCopy(Method method)
        : mMethod(method)
    {
    }

    std::remove_reference_t<Ret> operator()(Class &thiz, Args &&... args)
    {
        return (thiz.*mMethod)(std::forward<Args>(args)...);
    }

private:
    Method mMethod;
};




} // namespace saki



#endif // SAKI_DISMEMBER_H
