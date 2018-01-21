#ifndef SAKI_GIRLS_SHIRAITODAI_H
#define SAKI_GIRLS_SHIRAITODAI_H

#include "../table/irs_chance.h"



namespace saki
{



class Teru : public Girl
{
public:
    GIRL_CTORS(Teru)

    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onRoundEnded(const Table &table, RoundResult result,
                      const std::vector<Who> &openers, Who gunner,
                      const std::vector<Form> &fs) override;

    void nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;

private:
    TileCount mPlan;
    int mPrevGain = 0;
};



class Sumire : public Girl
{
public:
    GIRL_CTORS(Sumire)

    void onDice(util::Rand &rand, const Table &table) override;
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onIrsChecked(const Table &table, Mount &mount) override;

    std::string popUpStr() const override;

protected:
    IrsCtrlGetter attachIrsOnDrawn(const Table &table) override;

private:
    void handleDrawSelf(const Table &table, Mount &mount, bool rinshan);
    void handleDrawTarget(const Table &table, Mount &mount, bool rinshan);

    bool aimable(const Table &table);
    bool shootable(const Table &table);
    void pickBullet(const Table &table, Mount &mount);

    void shapeYaku(const Table &table, Mount &mount, bool rinshan);

private:
    // *** SYNC order with 'mIrsCtrl' ***
    enum IrsIndex { MAIN, RIGHT, CROSS, LEFT };

    IrsCtrlClickCheck<Sumire> mIrsCtrl {
        Choices::ModeIrsCheck {
            "SUMIRE",
            IrsCheckList {
                IrsCheckItem::CHECK_ENABLED,
                IrsCheckItem::CHILD_RADIO_DEFAULT,
                IrsCheckItem::CHILD_RADIO,
                IrsCheckItem::CHILD_RADIO,
            }
        },
        &Sumire::mIrsCtrl
    };

    Who mTarget;
    T34 mWant;
    T37 mFeed;
    int mShootTrial;
};



class Takami : public Girl
{
public:
    GIRL_CTORS(Takami)

    void onDiscarded(const Table &table, Who who) override;
    void nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;
    int d3gNeed(T34 t) const;

private:
    std::vector<T37> mSlots;
};



class Seiko : public Girl
{
public:
    GIRL_CTORS(Seiko)

    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    static const int ACCEL_MK = 500;
    static const int PAIR_MK = 400;
};



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



#endif // SAKI_GIRLS_SHIRAITODAI_H
