#ifndef SAKI_GIRL_SHIRAITODAI_TERU_H
#define SAKI_GIRL_SHIRAITODAI_TERU_H

#include "../table/girl.h"



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



} // namespace saki



#endif // SAKI_GIRL_SHIRAITODAI_TERU_H
