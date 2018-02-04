#ifndef SAKI_GIRL_X_YUI_H
#define SAKI_GIRL_X_YUI_H

#include "../table/girl.h"



namespace saki
{



class Yui : public Girl
{
public:
    GIRL_CTORS(Yui)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

};



} // namespace saki



#endif // SAKI_GIRL_X_YUI_H
