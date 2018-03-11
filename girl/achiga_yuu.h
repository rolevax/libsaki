#ifndef SAKI_GIRL_ACHIGA_YUU_H
#define SAKI_GIRL_ACHIGA_YUU_H

#include "../table/girl.h"



namespace saki
{



class Yuu : public Girl
{
public:
    GIRL_CTORS(Yuu)
    void onMonkey(std::array<Exist, 4> &exists, const Table &table) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    static const int LV5_VAL = 90;
    static const int LV4_VAL = 60;
    static const int LV3_VAL = 30;

    static const std::vector<T34> LV5_TARS;
    static const std::vector<T34> LV4_TARS;
    static const std::vector<T34> LV3_TARS;
};



} // namespace saki



#endif // SAKI_GIRL_ACHIGA_YUU_H
