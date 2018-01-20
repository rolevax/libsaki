#include "choices.h"



namespace saki
{



bool Choices::ModeBark::any() const
{
    return chiiL || chiiM || chiiR || pon || dmk || ron;
}



Choices::Choices()
{
    // leave the union garbage
}

bool Choices::any() const
{
    return mMode != Mode::WATCH;
}

Choices::Mode Choices::mode() const
{
    return mMode;
}

bool Choices::can(ActCode act) const
{
    using AC = ActCode;

    if (act == AC::IRS_CLICK)
        return mIrsClick;

    switch (mMode) {
    case Mode::WATCH:
        return false;
    case Mode::CUT:
        return act == AC::IRS_CHECK;
    case Mode::DICE:
        return act == AC::DICE;
    case Mode::DRAWN:
        switch (act) {
        case AC::SWAP_OUT:
        case AC::SPIN_OUT:
            return true;
        case AC::SWAP_RIICHI:
            return !mModeDrawn.swapRiichis.empty();
        case AC::SPIN_RIICHI:
            return mModeDrawn.spinRiichi;
        case AC::TSUMO:
            return mModeDrawn.tsumo;
        case AC::RYUUKYOKU:
            return mModeDrawn.kskp;
        case AC::ANKAN:
            return !mModeDrawn.ankans.empty();
        case AC::KAKAN:
            return !mModeDrawn.kakans.empty();
        default:
            return false;
        }

    case Mode::BARK:
        switch (act) {
        case AC::PASS:
            return true;
        case AC::CHII_AS_LEFT:
            return mModeBark.chiiL;
        case AC::CHII_AS_MIDDLE:
            return mModeBark.chiiM;
        case AC::CHII_AS_RIGHT:
            return mModeBark.chiiR;
        case AC::PON:
            return mModeBark.pon;
        case AC::DAIMINKAN:
            return mModeBark.dmk;
        case AC::RON:
            return mModeBark.ron;
        default:
            return false;
        }

    case Mode::END:
        switch (act) {
        case AC::NEXT_ROUND:
            return mModeEnd.next;
        case AC::END_TABLE:
            return mModeEnd.end;
        default:
            return false;
        }

    default:
        unreached("Choices::can");
    }
}

bool Choices::canRiichi() const
{
    return can(ActCode::SWAP_RIICHI) || can(ActCode::SPIN_RIICHI);
}

bool Choices::forwardAll() const
{
    return mForwardAll;
}

bool Choices::forwardAny() const
{
    return forwardAll() || mMode == Mode::CUT || mIrsClick;
}

bool Choices::spinOnly() const
{
    return mMode == Mode::DRAWN
           && !forwardAny()
           && !mModeDrawn.swapOut
           && !mModeDrawn.spinRiichi
           && !mModeDrawn.tsumo
           && !mModeDrawn.kskp
           && mModeDrawn.ankans.empty()
           && mModeDrawn.kakans.empty()
           && mModeDrawn.swapRiichis.empty();
}

Action Choices::sweep() const
{
    if (mForwardAll && mIrsClick)
        return Action(IRS_CLICK); // toki: exit future

    switch (mMode) {
    case Mode::WATCH:
        return Action();
    case Mode::CUT:
        // assume 0u is always legal
        return Action(ActCode::IRS_CHECK, 0u);
    case Mode::DICE:
        return Action(ActCode::DICE);
    case Mode::DRAWN:
        return Action(ActCode::SPIN_OUT);
    case Mode::BARK:
        return Action(ActCode::PASS);
    case Mode::END:
        return Action(mModeEnd.end ? ActCode::END_TABLE : ActCode::NEXT_ROUND);
    default:
        unreached("Choices::sweep");
    }
}

const Choices::ModeDrawn &Choices::drawn() const
{
    assert(mMode == Mode::DRAWN);
    return mModeDrawn;
}

const Choices::ModeBark &Choices::bark() const
{
    assert(mMode == Mode::BARK);
    return mModeBark;
}

void Choices::setCut()
{
    mMode = Mode::CUT;
}

void Choices::setDice()
{
    mMode = Mode::DICE;
}

void Choices::setDrawn(const Choices::ModeDrawn &mode)
{
    mMode = Mode::DRAWN;
    mModeDrawn = mode;
}

void Choices::setBark(const Choices::ModeBark &mode)
{
    if (mode.any()) {
        mMode = Mode::BARK;
        mModeBark = mode;
    } else {
        mMode = Mode::WATCH;
    }
}

void Choices::setEnd(const Choices::ModeEnd &mode)
{
    mMode = Mode::END;
    mModeEnd = mode;
}

void Choices::setExtra(bool v)
{
    mIrsClick = v;
}

void Choices::setForwarding(bool v)
{
    mForwardAll = v;
}



}  // namespace saki
