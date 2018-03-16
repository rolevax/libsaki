#ifndef SAKI_APP_GIRL_X_H
#define SAKI_APP_GIRL_X_H

#include "../table/girl.h"


namespace saki
{


class GirlX : public Girl
{
public:
    GIRL_COPY_CTORS(GirlX)

    GirlX(Who who);

    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



} // namespace saki



#endif // SAKI_APP_GIRL_X_H
