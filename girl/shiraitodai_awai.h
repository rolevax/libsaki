#ifndef SAKI_GIRL_SHIRAITODAI_AWAI_H
#define SAKI_GIRL_SHIRAITODAI_AWAI_H

#include "../table/princess.h"
#include "../table/irs_ctrl.h"



namespace saki
{



class Awai : public Girl, public HrhBargainer
{
public:
    GIRL_CTORS(Awai)

    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onMonkey(std::array<Exist, 4> &exists, const Table &table) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onFlipKandoraIndic(const Table &table, Mount &mount) override;
    void onDigUradoraIndic(const Table &table, Mount &mount, util::Stactor<Who, 4> openers) override;

    HrhBargainer *onHrhBargain(const Table &table) override;
    Claim hrhBargainClaim(int plan, T34 t) override;
    int hrhBargainPlanCt() override;
    void onHrhBargained(int plan, Mount &mount) override;
    std::optional<HrhInitFix> onHrhBeg(util::Rand &rand, const Table &table,
                                       const TileCount &stock) override;

protected:
    IrsCtrlGetter attachIrsOnDice() override;

private:
    struct InitSketch
    {
        std::array<T34, 3> heads;
        T34 pair;
        bool thirdIsTri;
        TileCount need;
    };

    bool usingDaburii() const;
    bool checkInitSelf(const Hand &init, int iter) const;
    bool checkInitRival(const Hand &init, int iter) const;
    T34 kanuraOfPlan(int plan) const;
    static int lastCorner(int dice, int kanCt);
    void begIter(HrhInitFix &fix, util::Rand &rand, const TileCount &stock);
    InitSketch sketch(util::Rand &rand, const util::Stactor<Suit, 3> &avaiSuits) const;
    bool pickWait(HrhInitFix &fix, util::Rand &rand, InitSketch &ske, const TileCount &stock);
    bool waitable(InitSketch &ske, const TileCount &stock, T34 wait);

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
    T34 mSomeGuestWind;
    T37 mFirstDraw;
    bool mNeedFirstDraw = false;
    T34 mKanura;
    std::vector<T34> mLastWaits;
};



} // namespace saki



#endif // SAKI_GIRL_SHIRAITODAI_AWAI_H
