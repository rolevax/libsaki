#ifndef SAKI_GIRL_X_KYOUKA_H
#define SAKI_GIRL_X_KYOUKA_H

#include "../table/girl.h"



namespace saki
{



class Kyouka : public Girl
{
public:
    GIRL_CTORS(Kyouka)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



} // namespace saki



#endif // SAKI_GIRL_X_KYOUKA_H
