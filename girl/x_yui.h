#ifndef SAKI_GIRL_X_YUI_H
#define SAKI_GIRL_X_YUI_H

#include "../table/girl.h"



namespace saki
{



class Yui : public GirlCrtp<Yui>
{
public:
    using GirlCrtp<Yui>::GirlCrtp;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

};



} // namespace saki



#endif // SAKI_GIRL_X_YUI_H
