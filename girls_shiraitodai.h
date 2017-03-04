#ifndef GIRLS_SHIRAITODAI_H
#define GIRLS_SHIRAITODAI_H

#include "girl.h"
#include "ticketfolder.h"



namespace saki
{



class Teru : public Girl
{
public:
    GIRL_CTORS(Teru)
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess);
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onRoundEnded(const Table &table, RoundResult result,
                      const std::vector<Who> &openers, Who gunner,
                      const std::vector<Form> &fs) override;

    void nonMonkey(Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;

private:
    TileCount mPlan;
    int mPrevGain = 0;
};



class Takami : public Girl
{
public:
    GIRL_CTORS(Takami)
    void onDiscarded(const Table &table, Who who) override;
    void nonMonkey(Rand &rand, TileCount &init, Mount &mount,
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

    void onDice(Rand &rand, const Table &table, TicketFolder &tickets) override;
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess);
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    void nonMonkey(Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;

    const IrsCheckRow &irsCheckRow(int index) const override;
    int irsCheckCount() const override;
    TicketFolder forwardAction(const Table &table, Mount &mount, const Action &action) override;

private:
    struct InitSketch
    {
        std::array<T34, 3> heads;
        T34 pair;
        bool thridIsTri;
        TileCount need;
    };

    static int lastCorner(int dice, int kanCt);
    InitSketch sketch(Rand &rand, const std::vector<Suit> &avaiSuits) const;
    bool pickWait(Rand &rand, InitSketch &ske, Mount &mount);

private:
    static const int DRAG_MK = 100;
    static const int ACCEL_MK = 800;
    static const int EJECT_MK = 100;

    TicketFolder mTicketsBackup;
    IrsCheckRow mDaburi { false, false, "AWAI_DABURI", true, false };
    T37 mFirstDraw;
    bool mNeedFirstDraw = false;
    T34 mKanura;
    std::vector<T34> mLastWaits;
};



} // namespace saki



#endif // GIRLS_SHIRAITODAI_H


