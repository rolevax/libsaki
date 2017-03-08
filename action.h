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
    NOTHING, PASS, SWAP_OUT, SPIN_OUT,
    CHII_AS_LEFT, CHII_AS_MIDDLE, CHII_AS_RIGHT,
    PON, DAIMINKAN, ANKAN, KAKAN, RIICHI, TSUMO,
    RON, RYUUKYOKU, END_TABLE, NEXT_ROUND, DICE,
    IRS_CHECK, IRS_CLICK, IRS_RIVAL,
    NUM_ACTCODE
};



static_assert(sizeof(int) >= 4, "action encoding");

class Action
{
public:
    Action();
    explicit Action(ActCode act);
    Action(ActCode act, int arg);
    Action(ActCode act, Who who);
    Action(ActCode act, const T37 &t);

    Action(const Action &copy) = default;
    Action &operator=(const Action &assign) = default;
    ~Action() = default;

    bool argIsTile() const;
    bool argIsShowAka5() const;

    ActCode act() const;
    const T37 &tile() const;
    int showAka5() const;
    int barkId() const;
    unsigned mask() const;
    Who rival() const;
    int encodeArg() const;

    bool isDiscard() const;
    bool isCpdmk() const;
    bool isIrs() const;

private:
    ActCode mAct;
    union
    {
        T37 mTile;
        int mArg;
        Who mWho;
    };
};



} // namespace saki



#endif // SAKI_ACTION_H


