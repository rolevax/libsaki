#include "form.h"
#include "explain.h"
#include "util.h"
#include "rand.h"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <cassert>



namespace saki
{



std::array<const char *, NUM_YAKUS> YAKU_STRS {
    "Rci", "Ipt", "Tmo", "Tny", "Pnf",
    "Y1y", "Y2y", "Y3y",
    "Y1f", "Y2f", "Y3f", "Y4f", // jkz and bkz has same strings
    "Y1f", "Y2f", "Y3f", "Y4f",
    "Ipk", "Rns", "Hai", "Hou", "Ckn",
    "Ssk", "Itt", "Cta",

    "Wri", "Ssk", "Itt", "Cta",
    "Toi", "Ctt", "Sak", "Skt", "Stk", "Hrt", "S3g", "H1t", "Jnc",

    "Mnh", "Jnc", "Rpk", "C1t", "Mnc",

    "X13", "Xd3", "X4a",  "Xt1", "Xs4", "Xd4",
    "Xcr", "Xr1", "Xth", "Xch", "X4k", "X9r",
    "W13", "W4a", "W9r"
};



Form::Form(const Hand &ready, const T37 &pick, const PointInfo &info, const RuleInfo &rule,
           const std::vector<T37> &drids, const std::vector<T37> &urids)
    : mDealerWin(info.selfWind == 1)
    , mRon(true)
    , mExtraRound(info.extraRound)
{
    initDora(drids, urids, ready, pick);

    if (ready.withPick(pick).step13() == -1) {
        init13(info, ready.closed(), pick);
    } else if (ready.withPick(pick).step4() == -1) {
        init4(info, rule, ready, pick);
    } else {
        // assume (?) 2pk is always bigger than 7-pairs
        assert(ready.withPick(pick).step7() == -1);
        init7(info, rule, ready.closed());
    }
}

Form::Form(const Hand &full, const PointInfo &info, const RuleInfo &rule,
           const std::vector<T37> &drids, const std::vector<T37> &urids)
    : mDealerWin(info.selfWind == 1)
    , mRon(false)
    , mExtraRound(info.extraRound)
{
    assert(full.hasDrawn()); // no drawn you tsumo a J8

    initDora(drids, urids, full);

    if (full.step13() == -1) {
        init13(info, full.closed(), full.drawn());
    } else if (full.step4() == -1) {
        init4(info, rule, full, full.drawn());
    } else {
        // assume (?) 2pk is always bigger than 7-pairs
        assert(full.step7() == -1);
        init7(info, rule, full.closed());
    }
}

bool Form::isPrototypalYakuman() const
{
    return mYakuman;
}

int Form::fu() const
{
    return mFu;
}

int Form::han() const
{
    return mHan;
}

int Form::base() const
{
    return mBase;
}

const Form::Yakus &Form::yakus() const
{
    return mYakus;
}

ManganType Form::manganType() const
{
    if (mBase == 2000)
        return ManganType::MG;
    if (mBase == 3000)
        return ManganType::HNM;
    if (mBase == 4000)
        return ManganType::BM;
    if (mBase == 6000)
        return ManganType::SBM;
    if (mBase == 8000)
        return ManganType::KZEYKM;
    return ManganType::HR;
}

bool Form::hasYaku() const
{
    return mYakus.any();
}

int Form::netLoss(bool explode) const
{
    int res;
    int b = base();

    res = mDealerWin ? (mRon ? b * 6 : b * 2)
                     : (mRon ? b * 4 : (explode ? b * 2 : b));

    if (res % 100 != 0)
        res = res - res % 100 + 100;

    return res;
}

int Form::netGain() const
{
    if (mRon) // ron: gain == loss
        return netLoss(false); // param 'explode' unused
    else if (mDealerWin) // dealer tsumo
        return 3 * netLoss(false);
    else // non-dealer tsumo
        return netLoss(true) + 2 * netLoss(false);
}

int Form::loss(bool explode) const
{
    return netLoss(explode) + (mExtraRound * (mRon ? 300 : 100));
}

int Form::gain() const
{
    if (mRon) // ron: gain == loss
        return loss(false); // param 'explode' unused
    else if (mDealerWin) // dealer tsumo
        return 3 * loss(false);
    else // non-dealer tsumo
        return loss(true) + 2 * loss(false);
}

std::string Form::spell() const
{
    std::ostringstream oss;

    if (mYakuman) {
        // forgot why I even append spaces at the end
        // maybe it does or does not matter for the gui layout
        for (int i = Yaku::KKSMS; i < NUM_YAKUS; i++)
            if (mYakus[i])
                oss << YAKU_STRS[i] << "  ";
    } else {
        Yakus copy = mYakus;
        if (copy[Yaku::RC] && copy[Yaku::TYC] && copy[Yaku::PF]) {
            copy.reset(Yaku::RC);
            copy.reset(Yaku::TYC);
            copy.reset(Yaku::PF);
            oss << "Mtp";
        }

        if (copy[Yaku::RC] && copy[Yaku::PF]) {
            copy.reset(Yaku::RC);
            copy.reset(Yaku::PF);
            oss << "Mpn";
        }

        if (copy[Yaku::RC] && copy[Yaku::TYC]) {
            copy.reset(Yaku::RC);
            copy.reset(Yaku::TYC);
            oss << "Mtn";
        }

        if (copy[Yaku::TYC] && copy[Yaku::PF]) {
            copy.reset(Yaku::TYC);
            copy.reset(Yaku::PF);
            oss << "Tpn";
        }

        if (copy[Yaku::RC] && copy[Yaku::MZCTMH]) {
            copy.reset(Yaku::RC);
            copy.reset(Yaku::MZCTMH);
            oss << "Rtm";
        }

        if (copy[Yaku::PF] && copy[Yaku::MZCTMH]) {
            copy.reset(Yaku::PF);
            copy.reset(Yaku::MZCTMH);
            oss << "Ptm";
        }

        if (copy[Yaku::TYC] && copy[Yaku::MZCTMH]) {
            copy.reset(Yaku::TYC);
            copy.reset(Yaku::MZCTMH);
            oss << "Ttm";
        }

        if (copy[Yaku::JKZ1F] && copy[Yaku::BKZ1F]) {
            copy.reset(Yaku::JKZ1F);
            copy.reset(Yaku::BKZ1F);
            oss << "W1f";
        }

        if (copy[Yaku::JKZ2F] && copy[Yaku::BKZ2F]) {
            copy.reset(Yaku::JKZ2F);
            copy.reset(Yaku::BKZ2F);
            oss << "W2f";
        }

        if (copy[Yaku::JKZ3F] && copy[Yaku::BKZ3F]) {
            copy.reset(Yaku::JKZ3F);
            copy.reset(Yaku::BKZ3F);
            oss << "W3f";
        }

        if (copy[Yaku::JKZ4F] && copy[Yaku::BKZ4F]) {
            copy.reset(Yaku::JKZ4F);
            copy.reset(Yaku::BKZ4F);
            oss << "W4f";
        }

        for (int i = 0; i < Yaku::KKSMS; i++)
            if (copy[i])
                oss << YAKU_STRS[i];

        int d = mDora;
        int u = mUradora;
        int a = mAkadora;

        if (d == 0 && u > 0 && a == 0)
            oss << "Ura" << u;
        else if (d == 0 && u == 0 && a > 0)
            oss << "Aka" << a;
        else if (d + u + a > 0)
            oss << "Dra" << (d + u + a);
        else if (mYakus.count() == 1)
            oss << "Nmi";
    }

    return oss.str();
}

std::string Form::charge() const
{
    std::ostringstream oss;

    const std::array<const char *, 6> manganName = {
        "", "Mg", "Hnm", "Bm", "Sbm", "Kzeykm"
    };

    if (mYakuman) {
        if (mExtraRound != 0)
            oss << mExtraRound << "Hb   ";
        oss << "Ykm ";
    } else {
        oss << mFu << "Fu ";
        oss << mHan << "Han";
        if (mExtraRound != 0)
            oss << " " << mExtraRound << "Hb";
        oss << "   " << manganName[manganType()];
    }

    if (mRon)
        oss << gain();
    else if (mDealerWin)
        oss << loss(false) << "All";
    else
        oss << loss(false) << "Dot" << loss(true);

    return oss.str();
}

void Form::init13(const PointInfo &info, const TileCount &ready, T34 last)
{
    mType = Type::F13;
    mYakuman = true;
    mBase = 8000;
    if (!mRon && info.bless) {
        // Tenhou or Chiihou
        mYakus.set(info.selfWind == 1 ? Yaku::TH : Yaku::CH);
        mYakus.set(Yaku::KKSMS_A);
    } else {
        mYakus.set(ready.ct(last) == 0 ? Yaku::KKSMS : Yaku::KKSMS_A);
    }
}

void Form::init4(const PointInfo &info, const RuleInfo &rule,
                 const Hand &hand, const T37 &last)
{
    mType = Type::F4;
    std::vector<Explain4> exps = Explain4::make(hand.closed(), hand.barks(),
                                                last, mRon);

    for (const Explain4 &exp : exps) {
        Yakus ykms = calcYakuman4(info, exp, hand.closed(), last);
        if (ykms.any()) {
            mYakuman = true;
            mYakus = ykms;
            mBase = 8000;
            break; // assume (?) yakumans are equivalently explained
        }

        Yakus ys = calcYaku4(info, rule, hand.isMenzen(), exp);
        int tempFu = calcFu(info, exp, hand.isMenzen());
        int tempHan = calcHan(ys);
        int tempBase = calcBase(tempFu, tempHan);
        if (tempBase > mBase
                || (tempBase == mBase && tempHan > mHan)
                || (tempBase == mBase && tempHan == mHan && tempFu > mFu)) {
            mYakus = ys;
            mBase = tempBase;
            mFu = tempFu;
            mHan = tempHan;
        }
    }
}

void Form::init7(const PointInfo &info, const RuleInfo &rule, const TileCount &ready)
{
    mType = Type::F7;

    // Tenhou Chiihou Tsuuiisou
    if (info.bless && !mRon) {
        mYakuman = true;
        mYakus.set(info.selfWind == 1 ? Yaku::TH : Yaku::CH);
    }

    if (ready.ctZ() == 13) {
        mYakuman = true;
        mYakus.set(Yaku::TIS);
    }

    if (mYakuman) {
        mBase = 8000;
        return;
    }

    mYakus.set(Yaku::CTT);
    checkPick(mYakus, info);
    checkRiichi(mYakus, info, rule);
    checkTsumo(mYakus, true);
    init7Dye(ready);
    init7TanyaoOrHonroutou(ready);

    mFu = 25;
    mHan = calcHan(mYakus);
    mBase = calcBase(mFu, mHan);
}

void Form::init7Dye(const TileCount &ready)
{
    bool hasM = false;
    for (int ti = 0; ti < 9; ti++) {
        if (ready.ct(T34(ti)) > 0) {
            hasM = true;
            break;
        }
    }

    bool hasP = false;
    for (int ti = 9; ti < 18; ti++) {
        if (ready.ct(T34(ti)) > 0) {
            hasP = true;
            break;
        }
    }

    bool hasS = false;
    for (int ti = 18; ti < 27; ti++) {
        if (ready.ct(T34(ti)) > 0) {
            hasS = true;
            break;
        }
    }

    if (hasM + hasP + hasS == 1)
        mYakus.set(ready.hasZ() ? Yaku::HIS : Yaku::CIS);
}

void Form::init7TanyaoOrHonroutou(const TileCount &ready)
{
    int yaoCt = ready.ctYao();
    if (yaoCt == 0)
        mYakus.set(Yaku::TYC);
    else if (yaoCt == 13)
        mYakus.set(Yaku::HRT);
}

void Form::initDora(const std::vector<T37> &drids, const std::vector<T37> &urids,
                    const Hand &full)
{
    mDora = drids % full;
    mUradora = urids % full;
    mAkadora = full.ctAka5();
}

void Form::initDora(const std::vector<T37> &drids, const std::vector<T37> &urids,
                    const Hand &ready, const T37 &last)
{
    mDora = (drids % ready) + (drids % last);
    mUradora = (urids % ready) + (urids % last);
    mAkadora = ready.ctAka5() + last.isAka5();
}

void Form::checkPick(Yakus &ys, const PointInfo &info) const
{
    if (info.duringKan)
        ys.set(mRon ? Yaku::CK : Yaku::RSKH);
    else if (info.emptyMount)
        ys.set(mRon ? Yaku::HTRY_R : Yaku::HTRY_T);
}

void Form::checkRiichi(Yakus &ys, const PointInfo &info, const RuleInfo &rule) const
{
    if (info.riichi != 0) {
        ys.set(info.riichi == 2 ? Yaku::DBRRC : Yaku::RC);
        if (rule.ippatsu && info.ippatsu)
            ys.set(Yaku::IPT);
    }
}

void Form::checkTsumo(Form::Yakus &ys, bool menzen) const
{
    if (!mRon && menzen)
        ys.set(Yaku::MZCTMH);
}

void Form::checkAge4(Form::Yakus &ys, const Explain4 &exp, bool menzen) const
{
    // init by pair's age
    bool all = exp.pair().isYao();
    bool none = !exp.pair().isYao();
    bool hasZ = exp.pair().isZ();

    for (auto it = exp.sb(); it != exp.se(); ++it) {
        bool yao = it->val() == 1 || it->val() == 7;
        all = all && yao;
        none = none && !yao;
        if (!all && !none) // mixture, no more hope
            return;
    }

    for (auto it = exp.x34b(); it != exp.x34e(); ++it) {
        bool yao = it->isYao();
        all = all && yao;
        none = none && !yao;
        if (!all && !none) // mixture, no more hope
            return;
        hasZ = hasZ || it->isZ();
    }

    if (none) {
        ys.set(Yaku::TYC);
    } else if (all) {
        if (exp.numX34() == 4) {
            assert(hasZ); // yakuman already checked
            ys.set(Yaku::HRT); // implies toitoi but does not care
        } else if (menzen) {
            ys.set(hasZ ? Yaku::HCTYC : Yaku::JCTYC);
        } else {
            ys.set(hasZ ? Yaku::HCTYC_K : Yaku::JCTYC_K);
        }
    }
}

void Form::checkPinfu4(Form::Yakus &ys, const PointInfo &info,
                       const Explain4 &exp, bool menzen) const
{
    if (menzen && exp.numS() == 4 && exp.wait() == Wait::BIFACE
            && !exp.pair().isYakuhai(info.selfWind, info.roundWind)) {
        ys.set(Yaku::PF);
    }
}

void Form::checkDye4(Form::Yakus &ys, const Explain4 &exp, bool menzen) const
{
    bool hasZ = exp.pair().isZ();
    std::array<bool, 3> hasNum { false, false, false };
    if (exp.pair().isNum())
        hasNum[static_cast<int>(exp.pair().suit())] = true;

    for (T34 t : exp.heads()) {
        if (t.isZ())
            hasZ = true;
        else
            hasNum[static_cast<int>(t.suit())] = true;
    }

    if (std::accumulate(hasNum.begin(), hasNum.end(), 0) == 1) {
        if (menzen)
            ys.set(hasZ ? Yaku::HIS : Yaku::CIS);
        else
            ys.set(hasZ ? Yaku::HIS_K : Yaku::CIS_K);
    }
}

void Form::checkCup4(Form::Yakus &ys, const Explain4 &exp, bool menzen) const
{
    if (!menzen)
        return;
    if (exp.numS() == 4
            && exp.heads().at(0) == exp.heads().at(1)
            && exp.heads().at(2) == exp.heads().at(3)) {
        ys.set(Yaku::RPK);
    } else if (exp.numS() >= 2) {
        for (auto it = exp.sb(); it + 1 != exp.se(); ++it) {
            if (*it == *(it + 1)) {
                ys.set(Yaku::IPK);
                break;
            }
        }
    }
}

void Form::checkYakuhai4(Form::Yakus &ys, const PointInfo &info, const Explain4 &exp) const
{
    for (auto it = exp.x34b(); it != exp.x34e(); ++it) {
        if (it->suit() == Suit::Y) {
            const std::array<Yaku, 3> Y { Yaku::YKH1Y, Yaku::YKH2Y, Yaku::YKH3Y };
            ys.set(Y.at(it->val() - 1));
        } else {
            if (it->suit() == Suit::F && it->val() == info.selfWind) {
                const std::array<Yaku, 4> J {
                    Yaku::JKZ1F, Yaku::JKZ2F, Yaku::JKZ3F, Yaku::JKZ4F,
                };
                ys.set(J.at(it->val() - 1));
            }

            if (it->suit() == Suit::F && it->val() == info.roundWind) {
                const std::array<Yaku, 4> B {
                    Yaku::BKZ1F, Yaku::BKZ2F, Yaku::BKZ3F, Yaku::BKZ4F,
                };
                ys.set(B.at(it->val() - 1));
            }
        }
    }
}

void Form::checkIttsuu4(Form::Yakus &ys, const Explain4 &exp, bool menzen) const
{
    auto check = [](T34 l, T34 m, T34 r) -> bool {
        return l.suit() == r.suit() && l.val() == 1 && m.val() == 4 && r.val() == 7;
    };

    const auto &h = exp.heads(); // save typing

    if (exp.numS() == 3) {
        if (check(h[0], h[1], h[2]))
            ys.set(menzen ? Yaku::IKTK : Yaku::IKTK_K);
    } else if (exp.numS() == 4) {
        if (check(h[0], h[1], h[2])
                || check(h[0], h[1], h[3])
                || check(h[0], h[2], h[3])
                || check(h[1], h[2], h[3]))
            ys.set(menzen ? Yaku::IKTK : Yaku::IKTK_K);
    }
}

void Form::checkSanshoku4(Form::Yakus &ys, const Explain4 &exp, bool menzen) const
{
    auto check = [](T34 l, T34 m, T34 r) -> bool {
        // inequality is transitive when it is sorted
        return l.suit() != m.suit() && m.suit() != r.suit()
                && l.val() == m.val() && m.val() == r.val();
    };

    const auto &h = exp.heads(); // save typing

    if (exp.numS() == 3) {
        if (check(h[0], h[1], h[2]))
            ys.set(menzen ? Yaku::SSKDJ : Yaku::SSKDJ_K);
    } else if (exp.numS() == 4) {
        if (check(h[0], h[1], h[2])
                || check(h[0], h[1], h[3])
                || check(h[0], h[2], h[3])
                || check(h[1], h[2], h[3]))
            ys.set(menzen ? Yaku::SSKDJ : Yaku::SSKDJ_K);
    }
}

void Form::checkX34s4(Form::Yakus &ys, const Explain4 &exp) const
{
    if (exp.numX34() == 4)
        ys.set(Yaku::TTH);
    if (exp.numC3() + exp.numC4() == 3)
        ys.set(Yaku::S3AK);
    if (exp.numO4() + exp.numC4() == 3)
        ys.set(Yaku::S3KT);
}

void Form::checkSanshokudoukou4(Form::Yakus &ys, const Explain4 &exp) const
{
    auto check = [](T34 l, T34 m, T34 r) -> bool {
        // suits must different (except vertical overflow bug)
        return l.isNum() && m.isNum() && r.isNum()
                && l.val() == m.val() && m.val() == r.val();
    };

    const auto &h = exp.heads(); // save typing

    if (exp.numX34() == 3) {
        if (check(h[1], h[2], h[3])) // X34 lays from the back
            ys.set(Yaku::SSKDK);
    } else if (exp.numX34() == 4) {
        if (check(h[0], h[1], h[2])
                || check(h[0], h[1], h[3])
                || check(h[0], h[2], h[3])
                || check(h[1], h[2], h[3]))
            ys.set(Yaku::SSKDK);
    }
}

void Form::checkShousangen(Form::Yakus &ys, const Explain4 &exp) const
{
    int ct = std::count_if(exp.x34b(), exp.x34e(),
                           [](T34 t) { return t.suit() == Suit::Y; });
    if (exp.pair().suit() == Suit::Y && ct == 2)
        ys.set(Yaku::SSG);
}

Form::Yakus Form::calcYakuman4(const PointInfo &info, const Explain4 &exp,
                               const TileCount &closed, const T37 &last) const
{
    Yakus res;
    bool anyWait = false;

    // Tenhou Chiihou Tsuuiisou
    if (info.bless && !mRon) {
        res.set(info.selfWind == 1 ? Yaku::TH : Yaku::CH);
        anyWait = true;
    }

    const std::array<T34, 4> &heads = exp.heads();
    if (util::all(heads, [](T34 h) { return h.isZ(); }) && exp.pair().isZ())
        res.set(Yaku::TIS);

    // Suuankou
    if (exp.numC3() + exp.numC4() == 4)
        res.set(exp.wait() == Wait::ISORIDE || anyWait ? Yaku::S4AK_A : Yaku::S4AK);

    // Daisangen
    int yCt = std::count_if(exp.x34b(), exp.x34e(),
                            [](T34 t) { return t.suit() == Suit::Y; });
    if (yCt == 3)
        res.set(Yaku::DSG);

    // Shousuushii Daisuushii
    int fCt = std::count_if(exp.x34b(), exp.x34e(),
                            [](T34 t) { return t.suit() == Suit::F; });
    if (fCt == 4)
        res.set(Yaku::DSS);
    else if (fCt == 3 && exp.pair().suit() == Suit::F)
        res.set(Yaku::SSS);

    // Chinroutou
    if (exp.numX34() == 4 && util::all(heads, [](T34 t) { return t.isNum19(); })
            && exp.pair().isNum19())
        res.set(Yaku::CRT);

    // Ryuuiisou
    auto green = [](T34 t) {
        if (t == T34(2, Suit::Y))
            return true;
        if (t.suit() != Suit::S)
            return false;
        int val = t.val();
        return val == 2 || val == 3 || val == 4 || val == 6 || val == 8;
    };
    auto greenSeq = [](T34 t) { return t == T34(2, Suit::S); };
    if (green(exp.pair())
            && util::all(exp.sb(), exp.se(), greenSeq)
            && util::all(exp.x34b(), exp.x34e(), green))
        res.set(Yaku::RIS);

    // Chuurenpoutou
    if (exp.pair().isNum()) {
        int_fast32_t totalLook = 0, waitLook = 0;
        Suit suit = exp.pair().suit();

        for (int val = 1; val <= 9; val++) {
            T34 t(suit, val);
            int waitCt = closed.ct(t);
            waitLook = waitLook * 10 + waitCt;
            totalLook = totalLook * 10 + waitCt + (t == last);
        }

        if (totalLook == 411111113 || totalLook == 321111113 ||
                 totalLook == 312111113 || totalLook == 311211113 ||
                 totalLook == 311121113 || totalLook == 311112113 ||
                 totalLook == 311111213 || totalLook == 311111123 ||
                 totalLook == 311111114)
            res.set(waitLook == 311111113 || anyWait ? Yaku::CRPT_A : Yaku::CRPT);
    }


    // Suukantsu
    if (exp.numO4() + exp.numC4() == 4)
        res.set(Yaku::S4KT);

    return res;
}

Form::Yakus Form::calcYaku4(const PointInfo &info, const RuleInfo &rule,
                            bool menzen, const Explain4 &exp) const
{
    Yakus res;

    checkPick(res, info);
    checkRiichi(res, info, rule);
    checkTsumo(res, menzen);
    checkAge4(res, exp, menzen);
    checkPinfu4(res, info, exp, menzen);
    checkDye4(res, exp, menzen);
    checkCup4(res, exp, menzen);
    checkYakuhai4(res, info, exp);
    checkIttsuu4(res, exp, menzen);
    checkSanshoku4(res, exp, menzen);
    checkX34s4(res, exp);
    checkSanshokudoukou4(res, exp);
    checkShousangen(res, exp);

    return res;
}

int Form::calcFu(const PointInfo &info, const Explain4 &exp, bool menzen) const
{
    int res = menzen && mRon ? 30 : 20;
    res += 2 * (exp.wait() == Wait::ISORIDE
                || exp.wait() == Wait::SIDE
                || exp.wait() == Wait::CLAMP);

    T34 p = exp.pair();
    // double-wind 4-fu superposition
    res += 2 * (p.suit() == Suit::F && p.val() == info.selfWind);
    res += 2 * (p.suit() == Suit::F && p.val() == info.roundWind);
    res += 2 * (p.suit() == Suit::Y);

    for (auto it = exp.o3b(); it != exp.o3e(); ++it)
        res += 2 + 2 * it->isYao();
    for (auto it = exp.c3b(); it != exp.c3e(); ++it)
        res += 4 + 4 * it->isYao();
    for (auto it = exp.o4b(); it != exp.o4e(); ++it)
        res += 8 + 8 * it->isYao();
    for (auto it = exp.c4b(); it != exp.c4e(); ++it)
        res += 16 + 16 * it->isYao();

    // tsumo without pinfu
    // rinshan is hard-coded to 2-fu
    res += 2 * (!mRon && !(res == 20 && menzen));

    if (res == 20 && mRon) // there's no 20-fu ron
        res = 30;
    if (res % 10 != 0) // 10-fu-ceiling
        res = res - res % 10 + 10;

    return res;
}

int Form::calcHan(const Yakus &ys) const
{
    int res = 0;

    int i = 0;

    while (i <= Yaku::HCTYC_K)
        res += ys[i++] * 1;

    while (i <= Yaku::JCTYC_K)
        res += ys[i++] * 2;

    while (i <= Yaku::RPK)
        res += ys[i++] * 3;

    while (i <= Yaku::CIS_K)
        res += ys[i++] * 5;

    while (i <= Yaku::CIS)
        res += ys[i++] * 6;

    res += mDora + mUradora + mAkadora;

    return res;
}

int Form::calcBase(int fu, int han) const
{
    int base;

    if (han <= 4) {
        base = fu << (2 + han);
        if (base >= 2000) {
            base = 2000;
        }
    } else if (han == 5) {
        base = 2000;
    } else if (han <= 7) {
        base = 3000;
    } else if (han <= 10) {
        base = 4000;
    } else if (han <= 12) {
        base = 6000;
    } else {
        base = 8000;
    }

    return base;
}



} // namespace saki


