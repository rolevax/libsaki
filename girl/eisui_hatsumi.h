#ifndef SAKI_GIRL_EISUI_HATSUMI_H
#define SAKI_GIRL_EISUI_HATSUMI_H

#include "../table/princess.h"



namespace saki
{



class Hatsumi : public Girl
{
public:
    GIRL_CTORS(Hatsumi)

    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    HrhInitFix *onHrhRaid(const Table &table) override;

private:
    HrhInitFix mRaider;
};



} // namespace saki



#endif // SAKI_GIRL_EISUI_HATSUMI_H
