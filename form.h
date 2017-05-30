#ifndef SAKI_FORM_H
#define SAKI_FORM_H

#include "hand.h"
#include "pointinfo.h"
#include "explain.h"

#include <bitset>



namespace saki
{



enum Yaku {
    // *** SYNC with Form::calcHan() ***
    // *** SYNC with YAKU_STRS and Form::spell() ***
    RC, IPT, MZCTMH, TYC, PF,
    YKH1Y, YKH2Y, YKH3Y,
    JKZ1F, JKZ2F, JKZ3F, JKZ4F,
    BKZ1F, BKZ2F, BKZ3F, BKZ4F,
    IPK, RSKH, HTRY_T, HTRY_R, CK,
    SSKDJ_K, IKTK_K, HCTYC_K,

    DBRRC, SSKDJ, IKTK, HCTYC,
    TTH, CTT, S3AK, S3KT, SSKDK, HRT, SSG, HIS_K, JCTYC_K,

    HIS, JCTYC, RPK, CIS_K, CIS,

    KKSMS, DSG, S4AK, TIS, SSS, DSS,
    CRT, RIS, TH, CH, S4KT, CRPT,
    KKSMS_A, S4AK_A, CRPT_A,

    NUM_YAKUS
};

enum ManganType { HR, MG, HNM, BM, SBM, KZEYKM };

extern std::array<const char *, NUM_YAKUS> YAKU_STRS;

class Form
{
public:
    enum class Type { F4, F7, F13 };

    using Yakus = std::bitset<Yaku::NUM_YAKUS>;

    Form(const Hand &ready, const T37 &pick, const PointInfo &info, const RuleInfo &rule,
         const util::Stactor<T37, 5> &drids = util::Stactor<T37, 5>(),
         const util::Stactor<T37, 5> &urids = util::Stactor<T37, 5>());
    Form(const Hand &full, const PointInfo &info, const RuleInfo &rule,
         const util::Stactor<T37, 5> &drids = util::Stactor<T37, 5>(),
         const util::Stactor<T37, 5> &urids = util::Stactor<T37, 5>());

    ~Form() = default;

    bool isPrototypalYakuman() const;
    int fu() const;
    int han() const;
    int base() const;
    int dora() const;
    int uradora() const;
    int akadora() const;
    const Yakus &yakus() const;
    std::vector<const char *> keys() const;
    ManganType manganType() const;
    bool hasYaku() const;

    int netLoss(bool isDealer) const;
    int netGain() const;
    int loss(bool isDealer) const;
    int gain() const;
    std::string spell() const;
    std::string charge() const;

private:
    void init13(const PointInfo &info, const TileCount &ready, T34 last);
    void init4(const PointInfo &info, const RuleInfo &rule, const Hand &hand, const T37 &last);
    void init7(const PointInfo &info, const RuleInfo &rule, const TileCount &ready);

    void init7Dye(const TileCount &ready);
    void init7TanyaoOrHonroutou(const TileCount &ready);
    void initDora(const util::Stactor<T37, 5> &drids, const util::Stactor<T37, 5> &urids,
                  const Hand &full);
    void initDora(const util::Stactor<T37, 5> &drids, const util::Stactor<T37, 5> &urids,
                  const Hand &ready, const T37 &last);

    void checkPick(Yakus &ys, const PointInfo &info) const;
    void checkRiichi(Yakus &ys, const PointInfo &info, const RuleInfo &rule) const;
    void checkTsumo(Yakus &ys, bool menzen) const;
    void checkAge4(Yakus &ys, const Explain4 &exp, bool menzen) const;
    void checkPinfu4(Yakus &ys, const PointInfo &info, const Explain4 &exp, bool menzen) const;
    void checkDye4(Yakus &ys, const Explain4 &exp, bool menzen) const;
    void checkCup4(Yakus &ys, const Explain4 &exp, bool menzen) const;
    void checkYakuhai4(Yakus &ys, const PointInfo &info, const Explain4 &exp) const;
    void checkIttsuu4(Yakus &ys, const Explain4 &exp, bool menzen) const;
    void checkSanshoku4(Yakus &ys, const Explain4 &exp, bool menzen) const;
    void checkX34s4(Yakus &ys, const Explain4 &exp) const;
    void checkSanshokudoukou4(Yakus &ys, const Explain4 &exp) const;
    void checkShousangen(Yakus &ys, const Explain4 &exp) const;

    Yakus calcYakuman4(const PointInfo &info, const Explain4 &exp,
                       const TileCount &closed, const T37 &last) const;
    Yakus calcYaku4(const PointInfo &info, const RuleInfo &rule,
                    bool menzen, const Explain4 &exp) const;
    int calcFu(const PointInfo &info, const Explain4 &exp, bool menzen) const;
    int calcHan(const Yakus &ys) const;
    int calcBase(int fu, int han) const;

private:
    Type mType;
    Yakus mYakus;
    bool mYakuman = false;
    bool mDealerWin;
    bool mRon;
    int mFu = 0;
    int mHan = 0;
    int mExtraRound;
    int mBase = 0;
    int mDora;
    int mUradora;
    int mAkadora;
};



} // namepace saki



#endif // SAKI_FORM_H


