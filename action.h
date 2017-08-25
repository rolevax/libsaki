#ifndef SAKI_ACTION_H
#define SAKI_ACTION_H

#include "tile.h"
#include "who.h"

#include <array>
#include <vector>



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
    Action();
    explicit Action(ActCode act);
    explicit Action(ActCode act, int arg);
    explicit Action(ActCode act, unsigned mask);
    explicit Action(ActCode act, const T37 &t);
    explicit Action(ActCode act, T34 t);
    explicit Action(ActCode act, int showAka5, const T37 &t);

    Action(const Action &copy) = default;
    Action &operator=(const Action &assign) = default;
    ~Action() = default;

    bool argIsOneT37() const;
    bool argIsOneIntegral() const;

    ActCode act() const;
    const T37 &t37() const;
    T34 t34() const;
    int showAka5() const;
    int barkId() const;
    unsigned mask() const;

    bool isDiscard() const;
    bool isRiichi() const;
    bool isChii() const;
    bool isCp() const;
    bool isCpdmk() const;
    bool isIrs() const;

    bool operator==(const Action &that) const;

    Action toRiichi() const;
    Action toDiscard() const;

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


