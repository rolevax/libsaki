#ifndef SAKI_GIRL_SHIRAITODAI_AWAI_H
#define SAKI_GIRL_SHIRAITODAI_AWAI_H

#include "../table/irs_ctrl.h"



namespace saki
{



class Awai : public Girl
{
public:
    GIRL_CTORS(Awai)

    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    void nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;

protected:
    IrsCtrlGetter attachIrsOnDice() override;

private:
    struct InitSketch
    {
        std::array<T34, 3> heads;
        T34 pair;
        bool thridIsTri;
        TileCount need;
    };

    static int lastCorner(int dice, int kanCt);
    InitSketch sketch(util::Rand &rand, const std::vector<Suit> &avaiSuits) const;
    bool pickWait(util::Rand &rand, InitSketch &ske, Mount &mount);

private:
    static const int DRAG_MK = 100;
    static const int ACCEL_MK = 800;
    static const int EJECT_MK = 100;

    IrsCtrlCheck<Awai> mIrsCtrl {
        Choices::ModeIrsCheck {
            "AWAI",
            IrsCheckList {
                IrsCheckItem::CHECK_ENABLED
            }
        }
    };
    T37 mFirstDraw;
    bool mNeedFirstDraw = false;
    T34 mKanura;
    std::vector<T34> mLastWaits;
};



} // namespace saki



#endif // SAKI_GIRL_SHIRAITODAI_AWAI_H
