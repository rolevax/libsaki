#include "form_gb.h"



namespace saki
{



FormGb::FormGb(const Hand &ready, const T37 &pick, const PointInfo &info)
{
    // TODO
    unreached("TODO FormGB ready-pick ctor");
}

FormGb::FormGb(const Hand &full, const PointInfo &info)
    : mDianpao(false)
{
    assert(full.hasDrawn()); // no drawn you zimo a J8

    if (full.step13() == -1) {
        init13(info);
    } else if (full.step4() == -1) {
        init4(info, full, full.drawn());
    } else {
        // assume (?) 2pk is always bigger than 7-pairs
        assert(full.step7() == -1);
        init7(info, full.closed());
    }
}

int FormGb::fan() const
{
    int res = 0;
    for (int i = Fan::DSY88; i <= Fan::SSY88; i++)
        res += 88 * mFans.test(i);
    return res;
}

void FormGb::init13(const PointInfo &info)
{
    mType = Type::F13;
    mFans.set(Fan::SSY88);
    checkPick(mFans, info);
}

void FormGb::init4(const PointInfo &info, const Hand &hand, const T37 &last)
{

}

void FormGb::init7(const PointInfo &info, const TileCount &ready)
{
    mType = Type::F7;
    // CONTINUE
}

void FormGb::checkPick(FormGb::Fans &fs, const PointInfo &info) const
{
    if (info.duringKan)
        fs.set(mDianpao ? Fan::QGH8 : Fan::GSKH8);
    else if (info.emptyMount)
        fs.set(mDianpao ? Fan::HDLY8 : Fan::MSHC8);
}



}


