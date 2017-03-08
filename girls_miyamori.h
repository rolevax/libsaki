#ifndef SAKI_GIRLS_MIYAMORI_H
#define SAKI_GIRLS_MIYAMORI_H

#include "girl.h"



namespace saki
{



class Toyone : public Girl
{
public:
    GIRL_CTORS(Toyone)

    void onDice(Rand &rand, const Table &table, TicketFolder &tickets) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onRiichiEstablished(const Table &table, Who who) override;

private:
    Who mFirstRiichi;
};



} // namespace saki



#endif // SAKI_GIRLS_MIYAMORI_H


