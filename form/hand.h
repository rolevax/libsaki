#ifndef SAKI_HAND_H
#define SAKI_HAND_H

#include "rule.h"
#include "form_ctx.h"
#include "tile_count.h"
#include "../unit/action.h"
#include "../unit/meld.h"



namespace saki
{



class Hand
{
public:
    Hand() = default;
    explicit Hand(const TileCount &count);
    explicit Hand(const TileCount &count, const util::Stactor<M37, 4> &barks);

    ~Hand() = default;

    Hand(const Hand &copy) = default;
    Hand(Hand &&move) = default;
    Hand &operator=(const Hand &assign) = default;
    Hand &operator=(Hand &&moveAssign) = default;

    const TileCount &closed() const;
    const T37 &drawn() const;
    const T37 &outFor(const Action &action) const;
    const util::Stactor<M37, 4> &barks() const;

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
    bool canCp(T34 pick, const Action &action) const;
    bool canDaiminkan(T34 t) const;
    bool canAnkan(util::Stactor<T34, 3> &choices, bool riichi) const;
    bool canKakan(util::Stactor<int, 3> &barkIds) const;
    bool canRon(T34 t, const FormCtx &info, const Rule &rule, bool &doujun) const;
    bool canTsumo(const FormCtx &info, const Rule &rule) const;
    bool canRiichi(util::Stactor<T37, 13> &swappables, bool &spinnable) const;

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

    util::Stactor<T34, 34> effA() const;
    util::Stactor<T34, 34> effA4() const;

    Parseds parse() const;
    Parsed4s parse4() const;

    int estimate(const Rule &rule, int sw, int rw, const util::Stactor<T37, 5> &drids) const;

    int peekPickStep(T34 pick) const;
    int peekPickStep4(T34 pick) const;
    int peekPickStep7(T34 pick) const;
    int peekPickStep7Gb(T34 pick) const;
    int peekPickStep13(T34 pick) const;

    template<typename Ret, typename... Params, typename... Args>
    Ret peekDiscard(const Action &a, Ret (Hand::*f) (Params...) const, Args && ... args) const
    {
        switch (a.act()) {
        case ActCode::SWAP_OUT:
            return peekSwap(a.t37(), f, std::forward<Args>(args) ...);
        case ActCode::SPIN_OUT:
            return peekSpin(f, std::forward<Args>(args) ...);
        default:
            unreached("Hand:withAction");
        }
    }

    template<typename Ret, typename... Params, typename... Args>
    Ret peekSwap(const T37 &t, Ret (Hand::*f) (Params...) const, Args && ... args) const
    {
        DeltaSwap guard(const_cast<Hand &>(*this), t);
        (void) guard;
        return (this->*f)(std::forward<Args>(args) ...);
    }

    template<typename Ret, typename... Params, typename... Args>
    Ret peekSpin(Ret (Hand::*f) (Params...) const, Args && ... args) const
    {
        DeltaSpin guard(const_cast<Hand &>(*this));
        (void) guard;
        return (this->*f)(std::forward<Args>(args) ...);
    }

    template<typename Ret, typename... Params, typename... Args>
    Ret peekCp(const T37 &pick, const Action &action,
               Ret (Hand::*f) (Params...) const, Args && ... args) const
    {
        assert(action.isCp());
        DeltaCp guard(const_cast<Hand &>(*this), pick, action, action.t37());
        (void) guard;
        return (this->*f)(std::forward<Args>(args) ...);
    }

    void draw(const T37 &in);
    void swapOut(const T37 &out);
    void spinOut();
    void barkOut(const T37 &out);
    void chiiAsLeft(const T37 &pick, bool showAka5);
    void chiiAsMiddle(const T37 &pick, bool showAka5);
    void chiiAsRight(const T37 &pick, bool showAka5);
    void pon(const T37 &pick, int showAka5, int layIndex);
    void daiminkan(const T37 &pick, int layIndex);
    void ankan(T34 t);
    void kakan(int barkId);

private:
    class DeltaSpin
    {
    public:
        explicit DeltaSpin(Hand &hand);
        ~DeltaSpin();

        DeltaSpin(const DeltaSpin &copy) = delete;
        DeltaSpin &operator=(const DeltaSpin &assign) = delete;

    private:
        Hand &mHand;
    };

    class DeltaSwap
    {
    public:
        explicit DeltaSwap(Hand &hand, const T37 &out);
        ~DeltaSwap();

        DeltaSwap(const DeltaSwap &copy) = delete;
        DeltaSwap &operator=(const DeltaSwap &assign) = delete;

    private:
        Hand &mHand;
        const T37 &mOut;
    };

    class DeltaCp
    {
    public:
        explicit DeltaCp(Hand &hand, const T37 &pick, const Action &a, const T37 &out);
        ~DeltaCp();

        DeltaCp(const DeltaCp &copy) = delete;
        DeltaCp &operator=(const DeltaCp &assign) = delete;

    private:
        Hand &mHand;
        const T37 &mOut;
    };

    using SwapOk = std::function<bool(T34)>;

    bool usingCache() const;
    const Parseds &loadCache() const;

    bool hasSwappableAfterChii(T34 mat1, T34 mat2, SwapOk ok) const;
    bool shouldShowAka5(T34 show, bool showAka5) const;
    T37 tryShow(T34 t, bool showAka5);
    util::Stactor<T37, 13> makeChoices(SwapOk ok) const;

    template<typename Ret, typename... Params, typename... Args>
    Ret peekStay(Ret (TileCount::*f)(Params...) const, Args... args) const
    {
        return mHasDrawn ? mClosed.peekDraw<Ret, Params...>(mDrawn, f, args...)
                         : (mClosed.*f)(args...);
    }

private:
    TileCount mClosed;
    T37 mDrawn;
    bool mHasDrawn = false;
    util::Stactor<M37, 4> mBarks;
    mutable std::optional<Parseds> mParseCache;
    mutable int mSkipCacheLevel = 0; ///< Ignore cache iff > 0
};

int operator%(T34 ind, const Hand &hand);
int operator%(const util::Stactor<T37, 5> &inds, const Hand &hand);



} // namespace saki



#endif // SAKI_HAND_H
