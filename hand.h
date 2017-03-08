#ifndef SAKI_HAND_H
#define SAKI_HAND_H

#include "tile.h"
#include "meld.h"
#include "tilecount.h"
#include "pointinfo.h"

#include <vector>
#include <functional>



namespace saki
{



class Hand;
class Action;

class HandDream
{
public:
    static const int STEP_INF = 14;

    static HandDream stay(const Hand &hand);
    static HandDream spin(const Hand &hand);
    static HandDream swap(const Hand &hand, const T37 &out);
    static HandDream pick(const Hand &hand, const T37 &in);

    bool ready() const;
    int step() const;
    int stepGb() const;
    int step4() const;
    int step7() const;
    int step7Gb() const;
    int step13() const;

    bool hasEffA(T34 t) const;
    bool hasEffA4(T34 t) const;
    bool hasEffA7(T34 t) const;
    bool hasEffA13(T34 t) const;

    std::vector<T34> effA() const;
    std::vector<T34> effA4() const;

    int doraValueBy(const std::vector<T37> &inds) const;
    int ctAka5() const;

    int estimate(const RuleInfo &rule, int sw, int rw, const std::vector<T37> &drids) const;

private:
    /// DRAWN_STAY drawn +1
    /// DRAWN_SPIN gar 0
    /// DRAWN_SWAP drawn +1, out -1
    /// PICK_STAY  gar 0
    /// PICK_IN    pick +1
    /// BARK_OUT   out -1
    /// BARK_STAY ...???
    enum class Type
    {
        DRAWN_STAY, DRAWN_SPIN, DRAWN_SWAP, PICK_STAY, PICK_IN, BARK_STAY, BARK_OUT
    };

    explicit HandDream(Type type, const Hand &hand, const T37 &tile);
    explicit HandDream(Type type, const Hand &hand);

    TileCount &enter() const;
    void leave() const;

private:
    Type mType;
    const Hand &mHand;
    T37 mTile; // either pick or out, not drawn
    bool mRon;
    mutable int mDepth = 0;
};



class Hand
{
public:
    Hand() = default;
    explicit Hand(const TileCount &count);
    explicit Hand(const TileCount &count, const std::vector<M37> &barks);

    ~Hand() = default;

    Hand(const Hand &copy) = default;
    Hand(Hand &&move) = default;
    Hand &operator=(const Hand &assign) = default;
    Hand &operator=(Hand &&moveAssign) = default;

    const TileCount &closed() const;
    const T37 &drawn() const;
    const std::vector<M37> &barks() const;

    bool hasDrawn() const;
    bool isMenzen() const;
    bool over4() const;
    bool nine9() const;

    int ct(T34 t) const;
    int ctAka5() const;

    bool canChii(T34 t) const;
    bool canChiiAsLeft(T34 t) const;
    bool canChiiAsMiddle(T34 t) const;
    bool canChiiAsRight(T34 t) const;
    bool canPon(T34 t) const;
    bool canDaiminkan(T34 t) const;
    bool canAnkan(std::vector<T34> &choices, bool riichi) const;
    bool canKakan(std::vector<int> &barkIds) const;
    bool canRon(T34 t, const PointInfo &info, const RuleInfo &rule, bool &doujun) const;
    bool canTsumo(const PointInfo &info, const RuleInfo &rule) const;
    bool canRiichi() const;

    bool ready() const;
    int step() const;
    int stepGb() const;
    int step4() const;
    int step7() const;
    int step7Gb() const;
    int step13() const;

    bool hasEffA(T34 t) const;
    bool hasEffA4(T34 t) const;
    bool hasEffA7(T34 t) const;
    bool hasEffA13(T34 t) const;

    std::vector<T34> effA() const;
    std::vector<T34> effA4() const;

    HandDream withAction(const Action &action) const;
    HandDream withPick(const T37 &pick) const;
    HandDream withSwap(const T37 &out) const;
    HandDream withSpin() const;

    void draw(const T37 &in);
    void swapOut(const T37 &out);
    void spinOut();
    std::vector<T37> chiiAsLeft(const T37 &pick, bool showAka5);
    std::vector<T37> chiiAsMiddle(const T37 &pick, bool showAka5);
    std::vector<T37> chiiAsRight(const T37 &pick, bool showAka5);
    std::vector<T37> pon(const T37 &pick, int showAka5, int layIndex);
    void daiminkan(const T37 &pick, int layIndex);
    void ankan(T34 t);
    void kakan(int barkId);
    void declareRiichi(std::vector<T37> &swappables, bool &spinnable) const;

    friend class HandDream;

private:
    using SwapOk = std::function<bool(T34)>;

    bool hasSwappableAfterChii(T34 mat1, T34 mat2, SwapOk ok) const;
    T37 tryShow(T34 t, bool showAka5);
    std::vector<T37> makeChoices(SwapOk ok) const;

private:
    mutable TileCount mCount;
    T37 mDrawn;
    bool mHasDrawn = false;
    std::vector<M37> mBarks;
};

int operator%(const T37 &ind, const Hand &hand);
int operator%(const std::vector<T37> &inds, const Hand &hand);
int operator%(const std::vector<T37> &inds, const HandDream &dream);



} // namespace saki



#endif // SAKI_HAND_H


