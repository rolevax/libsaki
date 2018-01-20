#ifndef SAKI_CHOICES_H
#define SAKI_CHOICES_H

#include "../unit/action.h"



namespace saki
{



class Choices
{
public:
    enum class Mode
    {
        WATCH, CUT, DICE, DRAWN, BARK, END
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
    bool forwardAll() const;
    bool forwardAny() const;
    bool spinOnly() const;

    Action sweep() const;

    const ModeDrawn &drawn() const;
    const ModeBark &bark() const;

    void setCut();
    void setDice();
    void setDrawn(const ModeDrawn &mode);
    void setBark(const ModeBark &mode);
    void setEnd(const ModeEnd &mode);

    void setExtra(bool v);
    void setForwarding(bool v);

private:
    Mode mMode = Mode::WATCH;
    bool mIrsClick = false;
    bool mForwardAll = false;

    union
    {
        ModeDrawn mModeDrawn;
        ModeBark mModeBark;
        ModeEnd mModeEnd;
    };

};



} // namespace saki



#endif // SAKI_CHOICES_H
