#ifndef SAKI_GIRL_SHIRAITODAI_TAKAMI_H
#define SAKI_GIRL_SHIRAITODAI_TAKAMI_H

#include "../table/princess.h"



namespace saki
{



class Takami : public Girl
{
public:
    GIRL_CTORS(Takami)

    HrhInitFix * onHrhRaid(const Table &table) override;
    void onTableEvent(const Table &table, const TableEvent &event) override;

    int d3gNeed(T34 t) const;

private:
    util::Stactor<T37, 13> mSlots;
    HrhInitFix mRaider;
};



} // namespace saki



#endif // SAKI_GIRL_SHIRAITODAI_TAKAMI_H
