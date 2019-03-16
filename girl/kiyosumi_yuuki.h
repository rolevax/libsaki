#ifndef SAKI_GIRL_KIYOSUMI_YUUKI_H
#define SAKI_GIRL_KIYOSUMI_YUUKI_H

#include "../table/girl.h"



namespace saki
{



class Yuuki : public GirlCrtp<Yuuki>
{
public:
    using GirlCrtp<Yuuki>::GirlCrtp;
    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onChooseFirstDealer(util::Rand &rand, Who tempDealer, int &die1, int &die2) override;
};



} // namespace saki



#endif // SAKI_GIRL_KIYOSUMI_YUUKI_H
