#ifndef SAKI_KAN_CTX_H
#define SAKI_KAN_CTX_H

#include "../unit/who.h"



namespace saki
{



class KanCtx
{
public:
    KanCtx() = default;
    explicit KanCtx(const KanCtx &copy) = default;
    KanCtx &operator=(const KanCtx &copy) = default;
    ~KanCtx() = default;

    bool during() const
    {
        return mDuring;
    }

    bool duringMinkan() const
    {
        return mDuring && mMinkan;
    }

    bool duringAnkan() const
    {
        return mDuring && !mMinkan;
    }

    Who pao() const
    {
        return mPao;
    }

    void enterDaiminkan(Who pao)
    {
        assert(!mDuring); // daiminkan must be a start

        mDuring = true;
        mMinkan = true;
        mPao = pao;
    }

    void enterAnkan()
    {
        if (!mDuring) { // start with ankan
            mPao = Who();
            mDuring = true;
        }

        mMinkan = false;
    }

    void enterKakan()
    {
        if (!mDuring) { // start with kakan
            mPao = Who();
            mDuring = true;
        }

        mMinkan = true;
    }

    /// \brief Called when a discard or another kan happens
    void leave()
    {
        mDuring = false;
    }

private:
    bool mDuring = false;
    bool mMinkan;
    Who mPao;
};



} // namespace saki



#endif // SAKI_KAN_CTX_H
