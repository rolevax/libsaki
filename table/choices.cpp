#include "choices.h"
#include "../util/dismember.h"
#include "../util/misc.h"



namespace saki
{



bool Choices::ModeIrsCheck::any() const
{
    return util::any(list, PredThis(&IrsCheckItem::able));
}



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

///
/// \brief Check ablity without nonce
/// \return true if the action in in the choice set
///
bool Choices::can(ActCode act) const
{
    using AC = ActCode;

    if (act == AC::IRS_CLICK)
        return mIrsClick;

    switch (mMode) {
    case Mode::WATCH:
        return false;
    case Mode::IRS_CHECK:
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

bool Choices::spinOnly() const
{
    return mMode == Mode::DRAWN
           && !mIrsClick
           && !mModeDrawn.swapOut
           && !mModeDrawn.spinRiichi
           && !mModeDrawn.tsumo
           && !mModeDrawn.kskp
           && mModeDrawn.ankans.empty()
           && mModeDrawn.kakans.empty()
           && mModeDrawn.swapRiichis.empty();
}

///
/// \brief Provide a fallback action in this choices.
///        Typically used as a placeholder on network timeout.
///
Action Choices::timeout() const
{
    // click whenever possible, to ensure toki's green world exited
    // clicking in other cases does no harm either
    if (can(ActCode::IRS_CLICK))
        return Action(ActCode::IRS_CLICK);

    switch (mMode) {
    case Mode::WATCH:
        return Action();
    case Mode::IRS_CHECK:
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

const Choices::ModeIrsCheck &Choices::irsCheck() const
{
    assert(mMode == Mode::IRS_CHECK);
    return mModeIrsCheck;
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

void Choices::setWatch()
{
    mMode = Mode::WATCH;
    mIrsClick = false;
}

/// allow to set all items disabled, without switching mode to 'watch'
void Choices::setIrsCheck(const ModeIrsCheck &irsCheck)
{
    mMode = Mode::IRS_CHECK;
    mModeIrsCheck = irsCheck;
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

void Choices::setIrsClick(const Choices &from)
{
    *this = from;
    mIrsClick = true;
}

void Choices::filter(const ChoiceFilter &f)
{
    switch (mode()) {
    case Mode::DRAWN:
        if (f.noTsumo)
            mModeDrawn.tsumo = false;

        if (f.noRiichi) {
            mModeDrawn.swapRiichis.clear();
            mModeDrawn.spinRiichi = false;
        }

        break;
    case Mode::BARK:
        if (f.noRon) {
            ModeBark mode = bark();
            mode.ron = false;
            setBark(mode);
        }

        break;
    default:
        break;
    }
}



}  // namespace saki
