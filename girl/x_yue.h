#ifndef SAKI_GIRL_X_YUE_H
#define SAKI_GIRL_X_YUE_H

#include "../table/girl.h"



namespace saki
{



class Yue : public GirlCrtp<Yue>
{
public:
    using GirlCrtp<Yue>::GirlCrtp;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    void dye(const TileCount &closed, Mount &mount, int mk);
    bool dyed(const Hand &hand);
    int countGuest(const Hand &hand, T34 g);
};



} // namespace saki



#endif // SAKI_GIRL_X_YUE_H
