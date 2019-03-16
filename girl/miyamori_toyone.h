#ifndef SAKI_GIRL_MIYAMORI_TOYONE_H
#define SAKI_GIRL_MIYAMORI_TOYONE_H

#include "../table/girl.h"



namespace saki
{



class Toyone : public GirlCrtp<Toyone>
{
public:
    using GirlCrtp<Toyone>::GirlCrtp;

    void onDice(util::Rand &rand, const Table &table) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    void onTableEvent(const Table &table, const TableEvent &event) override;

private:
    Who mFirstRiichi;
};



} // namespace saki



#endif // SAKI_GIRL_MIYAMORI_TOYONE_H
