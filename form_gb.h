#ifndef SAKI_FORM_GB_H
#define SAKI_FORM_GB_H

#include "hand.h"
#include "explain.h"

#include <functional>



namespace saki
{



enum Fan
{
    // *** SYNC with FormGb::fan() ***
    // *** SYNC with spell.js:fantr() ***
    DSY88, DSX88, JLBD88, LQD88, SG88, LYS88, SSY88,
    QYJ64, XSY64, XSX64, ZYS64, SAK64, YSSLH64,
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

    using Fans = std::vector<Fan>;

    FormGb(const Hand &ready, const T37 &pick, const PointInfo &info, bool juezhang);
    FormGb(const Hand &full, const PointInfo &info, bool juezhang);
    ~FormGb() = default;

    int fan() const;
    const Fans &fans() const;

private:
    void init13(const PointInfo &info);

    static int calcFan(const Fans &fs);

    Fans calcFansF7(const PointInfo &info, const Hand &hand) const;
    Fans calcFansF4(const PointInfo &info, const Hand &hand, const T37 &last,
                    const Explain4 &exp, bool juezhang) const;

    void checkV8864F4(Fans &res, const Hand &hand, const Explain4 &exp,
                      bool pure) const;
    void checkV4832F4(Fans &res, const Explain4 &exp, bool pureNumMelds) const;
    void checkV24F4(Fans &res, const Explain4 &exp, bool pure) const;
    void checkV16F4(Fans &res, const Explain4 &exp) const;
    void checkV12F4(Fans &res, const Explain4 &exp) const;
    void checkV8F4(Fans &res, const Explain4 &exp, const PointInfo &info) const;
    void checkV6F4(Fans &res, const Explain4 &exp, const Hand &hand) const;
    void checkV5F4(Fans &res, const Explain4 &exp) const;
    void checkV4F4(Fans &res, const Explain4 &exp, bool mqq, bool hjz) const;
    void checkV2F4(Fans &res, const Explain4 &exp, const PointInfo &info,
                   const Hand &hand, const T37 &pick) const;
    void checkV1F4(Fans &res, const Explain4 &exp, const PointInfo &info, const Hand &hand) const;

    void checkPick(Fans &fans, const PointInfo &info) const;

    bool seq3In3Or4(const Explain4 &exp, std::function<bool(T34, T34, T34)> p) const;

private:
    Type mType;
    bool mDianpao;
    Fans mFans;
    int mFan = 0;
};



} // namespace saki



#endif // SAKI_FORM_GB_H


