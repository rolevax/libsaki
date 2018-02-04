#ifndef SAKI_GIRL_HIMEMATSU_KYOUKO_H
#define SAKI_GIRL_HIMEMATSU_KYOUKO_H

#include "../table/girl.h"



namespace saki
{



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



#endif // SAKI_GIRL_HIMEMATSU_KYOUKO_H
