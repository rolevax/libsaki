#ifndef FORM_GB_H
#define FORM_GB_H

#include "hand.h"

#include <bitset>



namespace saki
{



enum Fan
{
    // *** SYNC with FormGb::fan() ***
    DSY88, DSS88, JLBD88, LQD88, SG88, LYS88, SSY88,
    QYJ64, XSY64, XSS64, ZYS64, SAK64, YSSLH64,
    YSSTS48, YSSJG48,
    YSSBG32, SG32, HYJ32,
    Q7D24, QSK24, QYS24, YSSTS24, YSSJG24, QDA24, QZ24, QX24,
    QL16, SSSLH16, YSSBG16, QDW16, STK16, SAK16,
    DYW12, XYW12, SFK12,
    HL8, TBD8, SSSTS8, SSSJG8, WFH8, MSHC8, HDLY8, GSKH8, QGH8,
    PPH6, HYS6, SSSBG6, WMQ6, QQR6, SAG6, SJK6,
    MAG5,
    QDY4, BQR4, SMG4, HJZ4,
    JK2, QFK2, MFK2, MQQ2, PH2, SGY2, STK2, SAK2, AG2, DY2,
    YBG1, XXF1, LL1, LSF1, YJK1, MG1, QYM1, WZ1, BZ1, KZ1, DDJ1, ZM1,
    NUM_FANS
};

class FormGb
{
public:
    enum class Type { F4, F7, F13 };

    using Fans = std::bitset<Fan::NUM_FANS>;

    FormGb(const Hand &ready, const T37 &pick, const PointInfo &info);
    FormGb(const Hand &full, const PointInfo &info);
    ~FormGb() = default;

    int fan() const;

private:
    void init13(const PointInfo &info);
    void init4(const PointInfo &info, const Hand &hand, const T37 &last);
    void init7(const PointInfo &info, const TileCount &ready);

    void checkPick(Fans &fans, const PointInfo &info) const;

private:
    Type mType;
    bool mDianpao;
    Fans mFans;
};



} // namespace saki



#endif // FORM_GB_H


