#ifndef SAKI_ACTION_H
#define SAKI_ACTION_H

#include "tile.h"
#include "who.h"



namespace saki
{



enum ActCode {
    // the order is priority, lower to higher, or does not matter
    // *** SYNC with stringOf(ActCode) ***
    NOTHING, PASS, SWAP_OUT, SPIN_OUT, SWAP_RIICHI, SPIN_RIICHI,
    CHII_AS_LEFT, CHII_AS_MIDDLE, CHII_AS_RIGHT,
    PON, DAIMINKAN, ANKAN, KAKAN, TSUMO, RON,
    RYUUKYOKU, END_TABLE, NEXT_ROUND, DICE,
    IRS_CHECK, IRS_CLICK,
    NUM_ACTCODE
};



class Action
{
public:
    Action()
        : mAct(ActCode::NOTHING)
    {
    }

    explicit Action(ActCode act)
        : mAct(act)
        , mArg(0) // use zero to guarentee equality
    {
        assert(!argIsOneT37()
               && !isCp()
               && mAct != ActCode::ANKAN
               && mAct != ActCode::KAKAN
               && mAct != ActCode::IRS_CHECK);
    }

    explicit Action(ActCode act, int arg)
        : mAct(act)
        , mArg(arg)
    {
        assert(mAct == ActCode::KAKAN);
    }

    explicit Action(ActCode act, unsigned mask)
        : mAct(act)
        , mMask(mask)
    {
        assert(mAct == ActCode::IRS_CHECK);
    }

    explicit Action(ActCode act, const T37 &t)
        : mAct(act)
        , mT37(t)
    {
        assert(argIsOneT37());
    }

    explicit Action(ActCode act, T34 t)
        : mAct(act)
        , mT34(t)
    {
        assert(mAct == ActCode::ANKAN);
    }

    explicit Action(ActCode act, int showAka5, const T37 &t)
        : mAct(act)
        , mArg(showAka5)
        , mT37(t)
    {
        assert(isCp());
    }

    Action(const Action &copy) = default;
    Action &operator=(const Action &assign) = default;
    ~Action() = default;

    bool argIsOneT37() const
    {
        return mAct == ActCode::SWAP_OUT || mAct == ActCode::SWAP_RIICHI;
    }

    bool argIsOneIntegral() const
    {
        return mAct == ActCode::KAKAN || mAct == ActCode::IRS_CHECK;
    }

    ActCode act() const
    {
        return mAct;
    }

    const T37 &t37() const
    {
        assert(argIsOneT37() || isCp());
        return mT37;
    }

    T34 t34() const
    {
        assert(mAct == ActCode::ANKAN);
        return mT34;
    }

    int showAka5() const
    {
        assert(isCp());
        return mArg;
    }

    int barkId() const
    {
        assert(mAct == ActCode::KAKAN);
        return mArg;
    }

    unsigned mask() const
    {
        assert(mAct == ActCode::IRS_CHECK);
        return mMask;
    }

    bool isDiscard() const
    {
        return mAct == ActCode::SWAP_OUT || mAct == ActCode::SPIN_OUT;
    }

    bool isRiichi() const
    {
        return mAct == ActCode::SWAP_RIICHI || mAct == ActCode::SPIN_RIICHI;
    }

    bool isChii() const
    {
        return mAct == ActCode::CHII_AS_LEFT
               || mAct == ActCode::CHII_AS_MIDDLE
               || mAct == ActCode::CHII_AS_RIGHT;
    }

    bool isCp() const
    {
        return mAct == ActCode::CHII_AS_LEFT
               || mAct == ActCode::CHII_AS_MIDDLE
               || mAct == ActCode::CHII_AS_RIGHT
               || mAct == ActCode::PON;
    }

    bool isCpdmk() const
    {
        return isCp() || mAct == ActCode::DAIMINKAN;
    }

    bool isIrs() const
    {
        return mAct == ActCode::IRS_CHECK || mAct == ActCode::IRS_CLICK;
    }

    bool operator==(const Action &that) const
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

    Action toRiichi() const
    {
        if (isRiichi())
            return *this;

        assert(isDiscard());
        return mAct == ActCode::SPIN_OUT ? Action(ActCode::SPIN_RIICHI)
                                         : Action(ActCode::SWAP_RIICHI, mT37);
    }

    Action toDiscard() const
    {
        if (isDiscard())
            return *this;

        assert(isRiichi());
        return mAct == ActCode::SPIN_RIICHI ? Action(ActCode::SPIN_OUT)
                                            : Action(ActCode::SWAP_OUT, mT37);
    }

private:
    ActCode mAct;
    union
    {
        int mArg;
        unsigned mMask;
    };
    union
    {
        T37 mT37;
        T34 mT34;
    };

    static_assert(sizeof(int) == sizeof(unsigned), "mArg representing ability");
};



} // namespace saki



#endif // SAKI_ACTION_H
