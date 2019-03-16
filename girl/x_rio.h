#ifndef SAKI_GIRL_X_RIO_H
#define SAKI_GIRL_X_RIO_H

#include "../table/girl.h"



namespace saki
{



class Rio : public GirlCrtp<Rio>
{
public:
    using GirlCrtp<Rio>::GirlCrtp;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



} // namespace saki



#endif // SAKI_GIRL_X_RIO_H
