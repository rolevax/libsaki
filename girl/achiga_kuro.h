#ifndef SAKI_GIRL_ACHIGA_KURO_H
#define SAKI_GIRL_ACHIGA_KURO_H

#include "../table/girl.h"



namespace saki
{



class Kuro : public Girl
{
public:
    GIRL_CTORS(Kuro)
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onDiscarded(const Table &table, bool spin) override;

private:
    static const int EJECT_MK = 5000;
    bool mCd = false;
};



} // namespace saki



#endif // SAKI_GIRL_ACHIGA_KURO_H
