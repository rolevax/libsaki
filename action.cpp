#include "action.h"
#include "util.h"

#include <numeric>
#include <cassert>



namespace saki
{



Action::Action()
    : mAct(ActCode::NOTHING)
{
}

Action::Action(ActCode act)
    : mAct(act)
    , mArg(0) // use zero to guarentee equality
{
    assert(!argIsOneT37()
           && !isCp()
           && mAct != ActCode::ANKAN
           && mAct != ActCode::KAKAN
           && mAct != ActCode::IRS_CHECK);
}

Action::Action(ActCode act, int arg)
    : mAct(act)
    , mArg(arg)
{
    assert(mAct == ActCode::KAKAN);
}

Action::Action(ActCode act, unsigned mask)
    : mAct(act)
    , mMask(mask)
{
    assert(mAct == ActCode::IRS_CHECK);
}

Action::Action(ActCode act, const T37 &t)
    : mAct(act)
    , mT37(t)
{
    assert(argIsOneT37());
}

Action::Action(ActCode act, T34 t)
    : mAct(act)
    , mT34(t)
{
    assert(mAct == ActCode::ANKAN);
}

Action::Action(ActCode act, int showAka5, const T37 &t)
    : mAct(act)
    , mArg(showAka5)
    , mT37(t)
{
    assert(isCp());
}

bool Action::argIsOneT37() const
{
    return mAct == ActCode::SWAP_OUT || mAct == ActCode::SWAP_RIICHI;
}

bool Action::argIsOneIntegral() const
{
    return mAct == ActCode::KAKAN || mAct == ActCode::IRS_CHECK;
}

ActCode Action::act() const
{
    return mAct;
}

const T37 &Action::t37() const
{
    assert(argIsOneT37() || isCp());
    return mT37;
}

T34 Action::t34() const
{
    assert(mAct == ActCode::ANKAN);
    return mT34;
}

int Action::showAka5() const
{
    assert(isCp());
    return mArg;
}

int Action::barkId() const
{
    assert(mAct == ActCode::KAKAN);
    return mArg;
}

unsigned Action::mask() const
{
    assert(mAct == ActCode::IRS_CHECK);
    return mMask;
}

bool Action::isDiscard() const
{
    return mAct == ActCode::SWAP_OUT || mAct == ActCode::SPIN_OUT;
}

bool Action::isRiichi() const
{
    return mAct == ActCode::SWAP_RIICHI || mAct == ActCode::SPIN_RIICHI;
}

bool Action::isChii() const
{
    return mAct == ActCode::CHII_AS_LEFT
            || mAct == ActCode::CHII_AS_MIDDLE
            || mAct == ActCode::CHII_AS_RIGHT;
}

bool Action::isCp() const
{
    return mAct == ActCode::CHII_AS_LEFT
            || mAct == ActCode::CHII_AS_MIDDLE
            || mAct == ActCode::CHII_AS_RIGHT
            || mAct == ActCode::PON;
}

bool Action::isCpdmk() const
{
    return isCp() || mAct == ActCode::DAIMINKAN;
}

bool Action::isIrs() const
{
    return mAct == ActCode::IRS_CHECK || mAct == ActCode::IRS_CLICK;
}

bool Action::operator==(const Action &that) const
{
    if (mAct != that.mAct)
        return false;

    if (argIsOneT37())
        return mT37 == that.mT37;
    if (isCp())
        return mArg == that.mArg && mT37 == that.mT37;
    if (argIsOneIntegral())
        return mArg == that.mArg;

    return true;
}

Action Action::toRiichi() const
{
    if (isRiichi())
        return *this;

    assert(isDiscard());
    return mAct == ActCode::SPIN_OUT ? Action(ActCode::SPIN_RIICHI)
                                     : Action(ActCode::SWAP_RIICHI, mT37);
}

Action Action::toDiscard() const
{
    if (isDiscard())
        return *this;

    assert(isRiichi());
    return mAct == ActCode::SPIN_RIICHI ? Action(ActCode::SPIN_OUT)
                                        : Action(ActCode::SWAP_OUT, mT37);
}



} // namespace saki


