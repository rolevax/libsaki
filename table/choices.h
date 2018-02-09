#ifndef SAKI_CHOICES_H
#define SAKI_CHOICES_H

#include "../unit/action.h"

#include <bitset>



namespace saki
{



class IrsCheckItem
{
public:
    IrsCheckItem() = default;

    enum Init
    {
        // Choices::sweep() of IRS_CHECK relys on the fact
        // that radio button can only be child of a check box
        // by assuming checking nothing is always a legal action
        CHECK_DISABLED, CHECK_ENABLED, CHILD_RADIO_DEFAULT, CHILD_RADIO
    };

    IrsCheckItem(Init config)
    {
        switch (config) {
        case CHECK_DISABLED:
            init(false, false, false, false);
            break;
        case CHECK_ENABLED:
            init(false, false, true, false);
            break;
        case CHILD_RADIO_DEFAULT:
            init(true, true, false, true);
            break;
        case CHILD_RADIO:
            init(true, true, false, false);
            break;
        default:
            break;
        }
    }

    bool mono() const
    {
        return mBits[MONO];
    }

    bool indent() const
    {
        return mBits[INDENT];
    }

    bool able() const
    {
        return mBits[ABLE];
    }

    bool on() const
    {
        return mBits[ON];
    }

    void setAble(bool v)
    {
        mBits[ABLE] = v;
    }

    void setOn(bool v)
    {
        mBits[ON] = v;
    }

private:
    void init(bool mono, bool indent, bool able, bool on)
    {
        mBits[MONO] = mono;
        mBits[INDENT] = indent;
        setAble(able);
        setOn(on);
    }

    enum Offset : unsigned { MONO, INDENT, ABLE, ON };

    std::bitset<4> mBits;
};

// assume 16 is enough
using IrsCheckList = util::Stactor<IrsCheckItem, 16>;



struct ChoiceFilter
{
    union
    {
        bool noTsumo = false;
        bool noRon;
    };

    bool noRiichi = false;

    void join(ChoiceFilter that)
    {
        noTsumo = noTsumo || that.noTsumo;
        noRiichi = noRiichi || that.noRiichi;
    }
};



///
/// \brief Choice set of one player
///
class Choices
{
public:
    enum class Mode
    {
        WATCH, IRS_CHECK, DICE, DRAWN, BARK, END
    };

    struct ModeIrsCheck
    {
        ModeIrsCheck(const char *n, IrsCheckList l) : name(n), list(l) {}
        const char *name = nullptr;
        IrsCheckList list;

        bool any() const;
    };

    struct ModeDrawn
    {
        bool swapOut = false;
        bool spinRiichi = false;
        bool tsumo = false;
        bool kskp = false;
        util::Stactor<T34, 3> ankans;
        util::Stactor<int, 3> kakans;
        util::Stactor<T37, 13> swapRiichis;
    };

    struct ModeBark
    {
        bool chiiL = false;
        bool chiiM = false;
        bool chiiR = false;
        bool pon = false;
        bool dmk = false;
        bool ron = false;
        util::Stactor<T37, 13> swapBarks;
        T34 focus;

        bool any() const;
    };

    struct ModeEnd
    {
        bool end = false;
        bool next = false;
    };

    Choices();

    bool any() const;

    Mode mode() const;
    bool can(ActCode act) const;
    bool canRiichi() const;
    bool spinOnly() const;

    Action timeout() const;

    const ModeIrsCheck &irsCheck() const;
    const ModeDrawn &drawn() const;
    const ModeBark &bark() const;

    void setWatch();
    void setIrsCheck(const ModeIrsCheck &irsCheck);
    void setDice();
    void setDrawn(const ModeDrawn &mode);
    void setBark(const ModeBark &mode);
    void setEnd(const ModeEnd &mode);

    void setIrsClick(const Choices &from);

    void filter(const ChoiceFilter &f);

private:
    Choices &operator=(const Choices &assign) = default;

private:
    Mode mMode = Mode::WATCH;
    bool mIrsClick = false;

    union
    {
        ModeIrsCheck mModeIrsCheck;
        ModeDrawn mModeDrawn;
        ModeBark mModeBark;
        ModeEnd mModeEnd;
    };

};



} // namespace saki



#endif // SAKI_CHOICES_H
