#ifndef SAKI_GIRL_KIYOSUMI_NODOKA_H
#define SAKI_GIRL_KIYOSUMI_NODOKA_H

#include "../table/girl.h"



namespace saki
{



class Nodoka : public Girl
{
public:
    GIRL_CTORS(Nodoka)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



} // namespace saki



#endif // SAKI_GIRL_KIYOSUMI_NODOKA_H
