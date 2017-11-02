#ifndef SAKI_GIRLS_ACHIGA_H
#define SAKI_GIRLS_ACHIGA_H

#include "girl.h"



namespace saki
{



class Kuro : public Girl
{
public:
    GIRL_CTORS(Kuro)
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onDiscarded(const Table &table, Who who) override;

private:
    static const int EJECT_MK = 5000;
    bool mCd = false;
};

class Yuu : public Girl
{
public:
    GIRL_CTORS(Yuu)
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    static const int LV5_VAL = 90;
    static const int LV4_VAL = 60;
    static const int LV3_VAL = 30;

    static const std::vector<T34> LV5_TARS;
    static const std::vector<T34> LV4_TARS;
    static const std::vector<T34> LV3_TARS;
};

class Ako : public Girl
{
public:
    GIRL_CTORS(Ako)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    static int sskDist(const TileCount &closed, T34 head);
    static int ittDist(const TileCount &closed, T34 head);
    static void oneDragTwo(Mount &mount, const TileCount &closed, T34 head);
    static void thinFill(Mount &mount, const TileCount &closed, T34 heads);
};



} // namespace saki



#endif // SAKI_GIRLS_ACHIGA_H


