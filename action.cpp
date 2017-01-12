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
    , mArg(0) // use zero to make 'encodeArg' equal
{
    assert(!argIsTile()
           && !argIsShowAka5()
           && mAct != ActCode::KAKAN
           && mAct != ActCode::IRS_CHECK
           && mAct != ActCode::IRS_RIVAL);
}

Action::Action(ActCode act, int arg)
    : mAct(act)
    , mArg(arg)
{
    assert(argIsShowAka5()
           || mAct == ActCode::KAKAN
           || mAct == ActCode::IRS_CHECK
           || mAct == ActCode::IRS_RIVAL);
}

Action::Action(ActCode act, const T37 &t)
    : mAct(act)
    , mTile(t)
{
    assert(argIsTile());
}

bool Action::argIsTile() const
{
    return mAct == ActCode::SWAP_OUT || mAct == ActCode::ANKAN;
}

bool Action::argIsShowAka5() const
{
    return mAct == ActCode::CHII_AS_LEFT
            || mAct == ActCode::CHII_AS_MIDDLE
            || mAct == ActCode::CHII_AS_RIGHT
            || mAct == ActCode::PON;
}

ActCode Action::act() const
{
    return mAct;
}

const T37 &Action::tile() const
{
    assert(argIsTile());
    return mTile;
}

int Action::showAka5() const
{
    assert(argIsShowAka5());
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
    return static_cast<unsigned>(mArg);
}

Who Action::rival() const
{
    assert(mAct == ActCode::IRS_RIVAL);
    return Who(mArg);
}

int Action::encodeArg() const
{
    if (mAct == ActCode::SWAP_OUT || mAct == ActCode::ANKAN)
        return mTile.id34() + (mTile.isAka5() ? 100 : 0);
    else
        return mArg;
}

bool Action::isDiscard() const
{
    return mAct == ActCode::SWAP_OUT || mAct == ActCode::SPIN_OUT;
}

bool Action::isCpdmk() const
{
    return mAct == ActCode::CHII_AS_LEFT
            || mAct == ActCode::CHII_AS_MIDDLE
            || mAct == ActCode::CHII_AS_RIGHT
            || mAct == ActCode::PON
            || mAct == ActCode::DAIMINKAN;
}

bool Action::isIrs() const
{
    return mAct == ActCode::IRS_CHECK
            || mAct == ActCode::IRS_CLICK
            || mAct == ActCode::IRS_RIVAL;
}



} // namespace saki


