#ifndef SAKI_GIRLS_HIMEMATSU_H
#define SAKI_GIRLS_HIMEMATSU_H

#include "girl.h"



namespace saki
{



class Suzu : public Girl
{
public:
    GIRL_CTORS(Suzu)
    void onDice(util::Rand &rand, const Table &table, Choices &choices) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    std::string popUpStr() const override;

private:
    static const std::array<int, 4> POWERS;
    static const int Z_POWER = 80;

    bool mExploded = false;
};



class Kyouko : public Girl
{
public:
    GIRL_CTORS(Kyouko)
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    enum Course { NONE, TANYAO, YAKUHAI };
    Course mCourse;
};



} // namespace saki



#endif // SAKI_GIRLS_HIMEMATSU_H


