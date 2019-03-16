#ifndef SAKI_GIRL_SHIRAITODAI_SEIKO_H
#define SAKI_GIRL_SHIRAITODAI_SEIKO_H

#include "../table/girl.h"



namespace saki
{



class Seiko : public GirlCrtp<Seiko>
{
public:
    using GirlCrtp<Seiko>::GirlCrtp;

    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    static const int ACCEL_MK = 500;
    static const int PAIR_MK = 400;
};



} // namespace saki



#endif // SAKI_GIRL_SHIRAITODAI_SEIKO_H
