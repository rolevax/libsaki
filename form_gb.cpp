#include "form_gb.h"
#include "util.h"



namespace saki
{



FormGb::FormGb(const Hand &ready, const T37 &pick, const PointInfo &info, bool juezhang)
    : mDianpao(true)
{
    if (ready.withPick(pick).step13() == -1) {
        init13(info);
    } else {
        if (ready.withPick(pick).step4() == -1) {
            std::vector<Explain4> exps = Explain4::make(ready.closed(), ready.barks(),
                                                        pick, mDianpao);

            for (const Explain4 &exp : exps) {
                Fans fs = calcFansF4(info, ready, pick, exp, juezhang);
                int tempFan = calcFan(fs);
                if (tempFan > mFan) {
                    mFans = fs;
                    mFan = tempFan;
                }
            }
        }

        if (ready.withPick(pick).step7Gb() == -1) {
            Fans fs = calcFansF7(info, ready);
            int tempFan = calcFan(fs);
            if (tempFan > mFan) {
                mFans = fs;
                mFan = tempFan;
            }
        }

        assert(mFan > 0);
    }
}

FormGb::FormGb(const Hand &full, const PointInfo &info, bool juezhang)
    : mDianpao(false)
{
    assert(full.hasDrawn()); // no drawn you zimo a J8

    if (full.step13() == -1) {
        init13(info);
    } else {
        if (full.step4() == -1) {
            std::vector<Explain4> exps = Explain4::make(full.closed(), full.barks(),
                                                        full.drawn(), mDianpao);

            for (const Explain4 &exp : exps) {
                Fans fs = calcFansF4(info, full, full.drawn(), exp, juezhang);
                int tempFan = calcFan(fs);
                if (tempFan > mFan) {
                    mFans = fs;
                    mFan = tempFan;
                }
            }
        }

        if (full.step7Gb() == -1) {
            Fans fs = calcFansF7(info, full);
            int tempFan = calcFan(fs);
            if (tempFan > mFan) {
                mFans = fs;
                mFan = tempFan;
            }
        }

        assert(mFan > 0);
    }
}

int FormGb::fan() const
{
    return mFan;
}

const FormGb::Fans &FormGb::fans() const
{
    return mFans;
}

void FormGb::init13(const PointInfo &info)
{
    mType = Type::F13;
    mFans.push_back(Fan::SSY88);
    checkPick(mFans, info);
    if (!mDianpao)
        mFans.push_back(Fan::ZM1);
    mFan = calcFan(mFans);
}

int FormGb::calcFan(const FormGb::Fans &fs)
{
    int res = 0;

    for (Fan f : fs) {
        if (Fan::DSY88 <= f && f <= Fan::SSY88)
            res += 88;
        else if (Fan::QYJ64 <= f && f <= Fan::YSSLH64)
            res += 64;
        else if (Fan::YSSTS48 <= f && f <= Fan::YSSJG48)
            res += 48;
        else if (Fan::YSSBG32 <= f && f <= Fan::HYJ32)
            res += 32;
        else if (Fan::Q7D24 <= f && f <= Fan::QX24)
            res += 24;
        else if (Fan::QL16 <= f && f <= Fan::SAK16)
            res += 16;
        else if (Fan::DYW12 <= f && f <= Fan::SFK12)
            res += 12;
        else if (Fan::HL8 <= f && f <= Fan::QGH8)
            res += 8;
        else if (Fan::PPH6 <= f && f <= Fan::SJK6)
            res += 12;
        else if (f == Fan::MAG5)
            res += 5;
        else if (Fan::QDY4 <= f && f <= Fan::HJZ4)
            res += 4;
        else if (Fan::JK2 <= f && f <= Fan::DY2)
            res += 2;
        else if (Fan::YBG1 <= f && f <= Fan::ZM1)
            res += 1;
        else
            unreached("FormGb::calcFan");
    }

    return res;
}

FormGb::Fans FormGb::calcFansF7(const PointInfo &info, const Hand &hand) const
{
    Fans res;

    // Lianqidui
    const std::vector<T34> ts = hand.closed().t34s();
    bool lqd = ts[0].suit() == ts.back().suit() && ts[0].val() + 6 == ts.back().val();
    if (lqd)
        res.push_back(Fan::LQD88);

    // Qidui
    if (!lqd)
        res.push_back(Fan::Q7D24);

    // Qingyise
    if (!lqd && ts.front().suit() == ts.back().suit())
        res.push_back(Fan::QYS24);

    // Quanda Quanzhong Quanxiao
    if (util::all(ts, [](T34 t) { return t.isNum() && t.val() >= 7; }))
        res.push_back(Fan::QDA24);
    if (util::all(ts, [](T34 t) { return t.isNum() && 4 <= t.val() && t.val() <= 6; }))
        res.push_back(Fan::QZ24);
    if (util::all(ts, [](T34 t) { return t.isNum() && t.val() <= 3; }))
        res.push_back(Fan::QX24);

    // Dayuwu Xiaoyuwu
    if (!util::has(res, Fan::QDA24)
            && util::all(ts, [](T34 t) { return t.isNum() && t.val() >= 5; })) {
        res.push_back(Fan::DYW12);
    }
    if (!util::has(res, Fan::QX24)
            && util::all(ts, [](T34 t) { return t.isNum() && t.val() <= 5; })) {
        res.push_back(Fan::XYW12);
    }

    // Tuibudao
    auto isTumbler = [](T34 t) {
        using namespace tiles34;
        const std::array<T34, 14> tumbler {
            1_p, 2_p, 3_p, 4_p, 5_p, 8_p, 9_p, 2_s, 4_s, 5_s, 6_s, 8_s, 9_s, 1_y
        };
        return util::has(tumbler, t);
    };
    if (util::all(ts, isTumbler))
        res.push_back(Fan::TBD8);

    checkPick(res, info);

    std::array<bool, 5> suits { false, false, false, false, false };
    for (T34 t : ts)
        suits[static_cast<int>(t.suit())] = true;

    // Hunyise
    if (suits[3] + suits[4] > 0 && suits[0] + suits[1] + suits[2] == 1)
        res.push_back(Fan::HYS6);

    // Wumenqi
    if (!util::has(suits, false))
        res.push_back(Fan::WMQ6);

    // Duanyao
    if (!util::has(res, Fan::QZ24)
            && util::none(ts, [](T34 t) { return t.isYao(); })) {
        res.push_back(Fan::DY2);
    }

    // Siguiyi
    if (!util::has(res, Fan::YSSTS48))
        for (int ti = 0; ti < 34; ti++)
            if (hand.closed().ct(T34(ti)) >= 3) // 4 or 3+pick
                res.push_back(Fan::SGY2);

    // Queyimen
    if (!util::has(res, Fan::TBD8) && suits[0] + suits[1] + suits[2] == 2)
        res.push_back(Fan::QYM1);

    // Wuzi
    const std::vector<Fan> implyWz {
        Fan::LQD88, Fan::QYS24, Fan::QDA24, Fan::QZ24, Fan::QX24,
        Fan::DYW12, Fan::XYW12, Fan::DY2
    };
    if (!util::common(res, implyWz)
            && util::none(ts, [](T34 t) { return t.isZ(); })) {
        res.push_back(Fan::WZ1);
    }

    // Zimo
    if (!mDianpao)
        res.push_back(Fan::ZM1);

    return res;
}

FormGb::Fans FormGb::calcFansF4(const PointInfo &info, const Hand &hand, const T37 &last,
                               const Explain4 &exp, bool juezhang) const
{
    TileCount total(hand.closed()); // copy
    for (const M37 &m: hand.barks())
        for (const T37 &t : m.tiles())
            total.inc(t, 1);
    total.inc(last, 1);

    Fans res;

    const std::array<T34, 4> &heads = exp.heads();

    bool pureNumMelds = heads[0].isNum()
            && heads[0].suit() == heads[1].suit()
            && heads[1].suit() == heads[2].suit()
            && heads[2].suit() == heads[3].suit();
    bool pure = pureNumMelds && heads[0].suit() == exp.pair().suit();

    checkV8864F4(res, hand, exp, pure);
    checkV4832F4(res, exp, pureNumMelds);
    checkV24F4(res, exp, pure);
    checkV16F4(res, exp);
    checkV12F4(res, exp);
    checkV8F4(res, exp, info);
    checkV6F4(res, exp, hand);
    checkV5F4(res, exp);
    checkV4F4(res, exp, hand.isMenzen() && !mDianpao, juezhang);
    checkV2F4(res, exp, info, hand, last);
    checkV1F4(res, exp, info, hand);
    if (res.empty()) // Wufanhu
        res.push_back(Fan::WFH8);


    return res;
}

void FormGb::checkV8864F4(Fans &res, const Hand &hand, const Explain4 &exp, bool pure) const
{
    const std::array<T34, 4> &heads = exp.heads();

    // Dasanyuan Xiaosanyuan
    int yCt = std::count_if(exp.x34b(), exp.x34e(),
                            [](T34 t) { return t.suit() == Suit::Y; });
    if (yCt == 3)
        res.push_back(Fan::DSY88);
    else if (yCt == 2 && exp.pair().suit() == Suit::Y)
        res.push_back(Fan::XSY64);

    // Dasixi Xiaosixi
    int fCt = std::count_if(exp.x34b(), exp.x34e(),
                            [](T34 t) { return t.suit() == Suit::F; });
    if (fCt == 4)
        res.push_back(Fan::DSX88);
    else if (fCt == 3 && exp.pair().suit() == Suit::F)
        res.push_back(Fan::XSX64);

    // Jiulianbaodeng
    if (hand.isMenzen() && pure) {
        int_fast32_t waitLook = 0;
        Suit suit = exp.pair().suit();
        for (int val = 1; val <= 9; val++)
            waitLook = waitLook * 10 + hand.closed().ct(T34(suit, val));

        if (waitLook == 311111113)
            res.push_back(Fan::JLBD88);
    }

    // Sigang
    if (exp.numO4() + exp.numC4() == 4)
        res.push_back(Fan::SG88);

    // Lvyise
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
        res.push_back(Fan::LYS88);

    // Qingyaojiu
    if (exp.numX34() == 4 && util::all(heads, [](T34 t) { return t.isNum19(); })
            && exp.pair().isNum19())
        res.push_back(Fan::QYJ64);

    // Ziyise
    if (util::all(heads, [](T34 h) { return h.isZ(); }) && exp.pair().isZ())
        res.push_back(Fan::ZYS64);

    // Si'anke
    if (exp.numC3() + exp.numC4() == 4)
        res.push_back(Fan::SAK64);

    // Yiseshuanglonghui
    if (pure && exp.numS() == 4 && exp.pair().val() == 5
            && heads[0].val() == 1 && heads[1].val() == 1
            && heads[2].val() == 7 && heads[3].val() == 7)
        res.push_back(Fan::YSSLH64);
}

void FormGb::checkV4832F4(Fans &res, const Explain4 &exp, bool pureNumMelds) const
{
    const std::array<T34, 4> &hs = exp.heads();
    std::array<int, 4> hvs;
    std::transform(hs.begin(), hs.end(), hvs.begin(),
                   [](T34 t) { return t.val(); });

    // Yisesitongshun
    if (pureNumMelds && exp.numS() == 4 && hvs[0] == hvs[3])
        res.push_back(Fan::YSSTS48);

    // Yisesijiegao
    if (pureNumMelds && exp.numX34() == 4) {
        std::array<T34, 4> xs(hs); // copy
        std::sort(xs.begin(), xs.end());
        if ((xs[0] | xs[1]) && (xs[1] | xs[2]) && (xs[2] | xs[3]))
            res.push_back(Fan::YSSJG48);
    }

    // Yisesibugao
    if (pureNumMelds && exp.numS() == 4) {
        bool twoJump = hvs[0] == 1 && hvs[1] == 3 && hvs[2] == 5 && hvs[3] == 7;
        bool oneJump = ((hs[0] | hs[1]) && (hs[1] | hs[2]) && (hs[2] | hs[3]));
        if (twoJump || oneJump)
            res.push_back(Fan::YSSBG32);
    }

    // Sangang
    if (exp.numC4() + exp.numO4() == 3)
        res.push_back(Fan::SG32);

    // Hunyaojiu
    const std::vector<Fan> implyHyj { Fan::QYJ64, Fan::ZYS64 };
    if (!util::common(res, implyHyj) && exp.numX34() == 4  && exp.pair().isYao()
            && util::all(hs, [](T34 t) { return t.isYao(); })) {
        res.push_back(Fan::HYJ32);
    }
}

void FormGb::checkV24F4(FormGb::Fans &res, const Explain4 &exp, bool pure) const
{
    const auto &heads = exp.heads();
    auto isDouble = [](T34 t) { return t.isNum() && t.val() % 2 == 0; };

    // Quanshuangke
    if (exp.numX34() == 4 && isDouble(exp.pair()) && util::all(heads, isDouble))
        res.push_back(Fan::QSK24);

    // Qingyise
    const std::vector<Fan> implyQys { Fan::JLBD88, Fan::YSSLH64 };
    if (!util::common(res, implyQys) && pure)
        res.push_back(Fan::QYS24);

    // Yisesantongshun
    if (!util::has(res, Fan::YSSTS48)) {
        bool a = exp.numS() >= 3 && heads[0] == heads[2];
        bool b = exp.numS() == 4 && heads[1] == heads[3];
        if (a || b)
            res.push_back(Fan::YSSTS24);
    }

    // Yisesanjiegao
    if (!util::has(res, Fan::YSSJG48) && exp.numX34() == 4) {
        std::vector<T34> xs(exp.x34b(), exp.x34e()); // copy
        std::sort(xs.begin(), xs.end());
        bool a = xs.size() >= 3 && (xs[0] | xs [1]) && (xs[1] | xs[2]);
        bool b = xs.size() == 4 && (xs[1] | xs [2]) && (xs[2] | xs[3]);
        if (a || b)
            res.push_back(Fan::YSSJG24);
    }

    // Quanda Quanzhong Quanxiao
    auto isBig = [](T34 t) { return t.isNum() && t.val() >= 7; };
    auto isMiddle = [](T34 t) { return t.isNum() && 4 <= t.val() && t.val() <= 6; };
    auto isSmall = [](T34 t) { return t.isNum() && t.val() <= 3; };
    if (isBig(exp.pair())
            && util::all(exp.sb(), exp.se(), [](T34 t) { return t.val() == 7; })
            && util::all(exp.x34b(), exp.x34e(), isBig)) {
        res.push_back(Fan::QDA24);
    } else if (isMiddle(exp.pair())
               && util::all(exp.sb(), exp.se(), [](T34 t) { return t.val() == 4; })
               && util::all(exp.x34b(), exp.x34e(), isMiddle)) {
        res.push_back(Fan::QZ24);
    } else if (isSmall(exp.pair())
               && util::all(exp.sb(), exp.se(), [](T34 t) { return t.val() == 1; })
               && util::all(exp.x34b(), exp.x34e(), isSmall)) {
        res.push_back(Fan::QX24);
    }
}

void FormGb::checkV16F4(FormGb::Fans &res, const Explain4 &exp) const
{
    const auto &h = exp.heads();

    // Qinglong
    auto ql = [](T34 l, T34 m, T34 r) {
        return l.suit() == r.suit() && l.val() == 1 && m.val() == 4 && r.val() == 7;
    };
    if (seq3In3Or4(exp, ql))
        res.push_back(Fan::QL16);

    // Sanseshuanglonghui
    if (exp.numS() == 4 && exp.pair().val() == 5
            && h[0].suit() == h[1].suit() && h[2].suit() == h[3].suit()
            && h[0].suit() != h[2].suit()
            && exp.pair().suit() != h[0].suit() && exp.pair().suit() != h[2].suit()
            && h[0].val() == 1 && h[1].val() == 7
            && h[2].val() == 1 && h[3].val() == 7) {
        res.push_back(Fan::SSSLH16);
    }

    // Yisesanbugao
    auto walk1 = [](T34 l, T34 m, T34 r) { return (l | m) && (m | r); };
    auto walk2 = [](T34 l, T34 m, T34 r) { return (l || m) && (m || r); };
    if (!util::has(res, Fan::YSSBG32))
        if (seq3In3Or4(exp, walk1) || seq3In3Or4(exp, walk2))
            res.push_back(Fan::YSSBG16);

    // Quandaiwu
    if (exp.pair().val() == 5
            && util::all(exp.sb(), exp.se(), [](T34 t) { return 3 <= t.val() && t.val() <= 5; })
            && util::all(exp.x34b(), exp.x34e(), [](T34 t) { return t.val() == 5; })) {
        res.push_back(Fan::QDW16);
    }

    // Santongke
    auto check = [](T34 l, T34 m, T34 r) -> bool {
        // suits must different (except vertical overflow bug)
        return l.isNum() && m.isNum() && r.isNum()
                && l.val() == m.val() && m.val() == r.val();
    };
    if (exp.numX34() == 3) {
        if (check(h[1], h[2], h[3])) // X34 lays from the back
            res.push_back(Fan::STK16);
    } else if (exp.numX34() == 4) {
        if (check(h[0], h[1], h[2])
                || check(h[0], h[1], h[3])
                || check(h[0], h[2], h[3])
                || check(h[1], h[2], h[3]))
            res.push_back(Fan::STK16);
    }

    // San'anke
    if (exp.numC3() + exp.numC4() == 3)
        res.push_back(Fan::SAK16);
}

void FormGb::checkV12F4(FormGb::Fans &res, const Explain4 &exp) const
{
    // Dayuwu
    auto gt5 = [](T34 t) { return t.isNum() && t.val() > 5; };
    if (!util::has(res, Fan::QDA24)
            && gt5(exp.pair())
            && util::all(exp.sb(), exp.se(), [](T34 t) { return t.val() > 5; })
            && util::all(exp.x34b(), exp.x34e(), gt5)) {
        res.push_back(Fan::DYW12);
    }

    // Xiaoyuwu
    auto lt5 = [](T34 t) { return t.isNum() && t.val() < 5; };
    if (!util::has(res, Fan::QX24)
            && lt5(exp.pair())
            && util::all(exp.sb(), exp.se(), [](T34 t) { return t.val() < 2; })
            && util::all(exp.x34b(), exp.x34e(), lt5)) {
        res.push_back(Fan::XYW12);
    }

    // Sanfengke
    auto isF = [](T34 t) { return t.suit() == Suit::F; };
    if (!util::has(res, Fan::XSX64) && std::count_if(exp.x34b(), exp.x34e(), isF) == 3)
        res.push_back(Fan::SFK12);
}

void FormGb::checkV8F4(FormGb::Fans &res, const Explain4 &exp, const PointInfo &info) const
{
    const std::array<T34, 4> &h = exp.heads();

    // Hualong
    std::vector<T34> vals(exp.sb(), exp.se()); // copy
    std::sort(vals.begin(), vals.end(), [](T34 a, T34 b) { return a.val() < b.val(); });
    auto jerk = [](T34 a, T34 b, T34 c) {
        // ineqaulity is transitive if ordered
        return a.suit() != b.suit() && b.suit() != c.suit()
                && a.val() + 3 == b.val() && b.val() + 3 == c.val();
    };
    if (exp.numS() == 3) {
        if (jerk(h[0], h[1], h[2]))
            res.push_back(Fan::HL8);
    } else if (exp.numS() == 4) {
        if (jerk(h[0], h[1], h[2])
                || jerk(h[0], h[1], h[3])
                || jerk(h[0], h[2], h[3])
                || jerk(h[1], h[2], h[3]))
            res.push_back(Fan::HL8);
    }

    // Tuibudao
    using namespace tiles34;
    auto isTumblerSeq = [](T34 t) {
        const std::array<T34, 4> tumblerSeq { 1_p, 2_p, 3_p, 4_s };
        return util::has(tumblerSeq, t);
    };
    auto isTumbler = [](T34 t) {
        const std::array<T34, 14> tumbler {
            1_p, 2_p, 3_p, 4_p, 5_p, 8_p, 9_p, 2_s, 4_s, 5_s, 6_s, 8_s, 9_s, 1_y
        };
        return util::has(tumbler, t);
    };
    if (isTumbler(exp.pair())
            && util::all(exp.sb(), exp.se(), isTumblerSeq)
            && util::all(exp.x34b(), exp.x34e(), isTumbler)) {
        res.push_back(Fan::TBD8);
    }

    // Sansesantongshun
    auto sanse = [](T34 a, T34 b, T34 c) {
        return a.suit() != b.suit() && b.suit() != c.suit()
                && a.val() == b.val() && b.val() == c.val();
    };
    if (seq3In3Or4(exp, sanse))
        res.push_back(Fan::SSSTS8);

    // Sansesanjiegao
    std::vector<T34> xs(exp.x34b(), exp.x34e()); // copy
    std::sort(xs.begin(), xs.end(), [](T34 a, T34 b) { return a.val() < b.val(); });
    auto kick = [](T34 a, T34 b, T34 c) {
        return a.suit() != b.suit() && b.suit() != c.suit()
                && a.val() + 1 == b.val() && b.val() + 1 == c.val();
    };
    if (exp.numX34() == 3) {
        if (kick(h[1], h[2], h[3])) // X34s lay from the back
            res.push_back(Fan::SSSJG8);
    } else if (exp.numX34() == 4) {
        if (kick(h[0], h[1], h[2])
                || kick(h[0], h[1], h[3])
                || kick(h[0], h[2], h[3])
                || kick(h[1], h[2], h[3]))
            res.push_back(Fan::SSSJG8);
    }

    checkPick(res, info);
}

void FormGb::checkV6F4(FormGb::Fans &res, const Explain4 &exp, const Hand &hand) const
{
    const std::array<T34, 4> &h = exp.heads();

    // Pengpenghu
    const std::vector<Fan> implyPph {
        Fan::DSX88, Fan::SG88, Fan::QYJ64, Fan::ZYS64, Fan::SAK64,
        Fan::YSSJG48, Fan::HYJ32, Fan::QSK24
    };
    if (!util::common(res, implyPph) && exp.numX34() == 4)
        res.push_back(Fan::PPH6);

    std::array<bool, 5> suits { false, false, false, false, false };
    suits[static_cast<int>(exp.pair().suit())] = true;
    for (T34 t : h)
        suits[static_cast<int>(t.suit())] = true;

    // Hunyise
    if (!util::has(res, Fan::LYS88)
            && suits[3] + suits[4] > 0 && suits[0] + suits[1] + suits[2] == 1) {
        res.push_back(Fan::HYS6);
    }

    // Sansesanbubao
    std::vector<T34> seqs(exp.sb(), exp.se()); // copy
    std::sort(seqs.begin(), seqs.end(), [](T34 a, T34 b) { return a.val() < b.val(); });
    auto raise = [](T34 a, T34 b, T34 c) {
        return a.suit() != b.suit() && b.suit() != c.suit()
                && a.val() + 1 == b.val() && b.val() + 1 == c.val();
    };
    if (exp.numS() == 3) {
        if (raise(h[0], h[1], h[2]))
            res.push_back(Fan::SSSBG6);
    } else if (exp.numS() == 4) {
        if (raise(h[0], h[1], h[2])
                || raise(h[0], h[1], h[3])
                || raise(h[0], h[2], h[3])
                || raise(h[1], h[2], h[3]))
            res.push_back(Fan::SSSBG6);
    }

    // Wumenqi
    if (!util::has(suits, false))
        res.push_back(Fan::WMQ6);

    // Quanqiuren
    auto isAnkan = [](const M37 &m) { return m.type() == M37::Type::ANKAN; };
    if (hand.barks().size() == 4 && util::none(hand.barks(), isAnkan))
        res.push_back(Fan::QQR6);

    // Shuang'an'gang
    const std::vector<Fan> implySag { Fan::SG88, Fan::SG32, Fan::SAK2 };
    if (!util::common(res, implySag) && exp.numC4() == 2)
        res.push_back(Fan::SAG6);

    // Shuangjianke
    int yCt = std::count_if(exp.x34b(), exp.x34e(), [](T34 t) { return t.suit() == Suit::Y; });
    if (!util::has(res, Fan::XSY64) && yCt == 2)
        res.push_back(Fan::SJK6);
}

void FormGb::checkV5F4(FormGb::Fans &res, const Explain4 &exp) const
{
    // Ming'an'gang
    const std::vector<Fan> implyMag { Fan::SG88, Fan::SG32 };
    if (!util::common(res, implyMag) && exp.numO4() == 1 && exp.numC4() == 1)
        res.push_back(Fan::MAG5);
}

void FormGb::checkV4F4(FormGb::Fans &res, const Explain4 &exp, bool mqq, bool hjz) const
{
    // Quandaiyao
    std::vector<Fan> implyQdy { Fan::QYJ64, Fan::ZYS64, Fan::HYJ32 };
    if (!util::common(res, implyQdy)
            && exp.pair().isYao()
            && util::all(exp.sb(), exp.se(), [](T34 t) { return t.val() == 1 || t.val() == 7; })
            && util::all(exp.x34b(), exp.x34e(), [](T34 t) { return t.isYao(); })) {
        res.push_back(Fan::QDY4);
    }

    // Buqiuren
    std::vector<Fan> implyBqr { Fan::JLBD88, Fan::SAK64 };
    if (!util::common(res, implyBqr) && mqq && !mDianpao)
        res.push_back(Fan::BQR4);

    // Shuangming'gang
    const std::vector<Fan> implySmg { Fan::SG88, Fan::SG32 };
    if (!util::common(res, implySmg) && exp.numO4() == 2)
        res.push_back(Fan::SMG4);

    // Hujuezhang
    if (hjz)
        res.push_back(Fan::HJZ4);
}

void FormGb::checkV2F4(FormGb::Fans &res, const Explain4 &exp,
                       const PointInfo &info, const Hand &hand, const T37 &pick) const
{
    // Jianke
    int yCt = std::count_if(exp.x34b(), exp.x34e(),
                            [](T34 t) { return t.suit() == Suit::Y; });
    if (yCt == 1)
        res.push_back(Fan::JK2);

    // Quanfengke
    T34 quanF(Suit::F, info.roundWind);
    if (!util::has(res, Fan::DSX88) && util::has(exp.x34b(), exp.x34e(), quanF))
        res.push_back(Fan::QFK2);

    // Menfengke
    T34 menF(Suit::F, info.selfWind);
    if (!util::has(res, Fan::DSX88) && util::has(exp.x34b(), exp.x34e(), menF))
        res.push_back(Fan::MFK2);

    // Menqianqing
    const std::vector<Fan> implyMqq { Fan::JLBD88, Fan::SAK64, Fan::BQR4 };
    if (!util::common(res, implyMqq) && hand.isMenzen())
        res.push_back(Fan::MQQ2);

    // Pinghu
    const std::vector<Fan> implyPh { Fan::YSSLH64, Fan::SSSLH16 };
    if (!util::common(res, implyPh) && exp.numS() == 4 && exp.pair().isNum())
        res.push_back(Fan::PH2);

    // Siguiyi
    TileCount noGang(hand.closed()); // copy
    for (const M37 &m : hand.barks())
        if (!m.isKan())
            for (const T37 &t : m.tiles())
                noGang.inc(t, 1);
    noGang.inc(pick, 1);
    for (int ti = 0; ti < 34; ti++)
        if (noGang.ct(T34(ti)) == 4)
            res.push_back(Fan::SGY2);

    // Shuangtongke
    if (!util::has(res, Fan::QYJ64))
        for (auto it = exp.x34b(); it + 1 < exp.x34e(); it++)
            for (auto jt = it + 1; jt < exp.x34e(); jt++)
                if (it->isNum() && jt->isNum() && it->val() == jt->val())
                    res.push_back(Fan::STK2);

    // Shuanganke
    if (exp.numC3() + exp.numC4() == 2)
        res.push_back(Fan::SAK2);

    // An'gang
    const std::vector<Fan> implyAg { Fan::SG88, Fan::SG32 };
    if (!util::common(res, implyAg) && exp.numC4() == 1)
        res.push_back(Fan::AG2);

    // Duanyao
    const std::vector<Fan> implyDy { Fan::QSK24, Fan::QZ24, Fan::QDW16 };
    if (!util::common(res, implyDy)
            && util::none(exp.sb(), exp.se(), [](T34 t) { return t.val() == 1 || t.val() == 7; })
            && util::none(exp.x34b(), exp.x34e(), [](T34 t) { return t.isYao(); })
            && !exp.pair().isYao()) {
        res.push_back(Fan::DY2);
    }
}

void FormGb::checkV1F4(FormGb::Fans &res, const Explain4 &exp,
                       const PointInfo &info, const Hand &hand) const
{
    // Yibangao
    // Xixiangfeng
    // Lianliu
    // Laoshaofu
    const std::vector<Fan> implyYbg { Fan::YSSLH64, Fan::YSSTS48, Fan::YSSTS24 };
    const std::vector<Fan> implyXxf { Fan::SSSLH16, Fan::SSSTS8 };
    const std::vector<Fan> implyLl { Fan::QL16 };
    const std::vector<Fan> implyLsf { Fan::YSSLH64, Fan::QL16, Fan::SSSLH16 };
    auto exclude = [](T34, T34) { return false; };
    auto ban = util::common(res, implyYbg) ? exclude
                                           : [](T34 a, T34 b) { return a == b; };
    auto feng = util::common(res, implyXxf) ? exclude
                                            : [](T34 a, T34 b) {
        return a.suit() != b.suit() && a.val() == b.val();
    };
    auto lian = util::common(res, implyLl) ? exclude
                                           : [](T34 a, T34 b) { return a ^ b; };
    auto lao = util::common(res, implyLsf) ? exclude
                                           : [](T34 a, T34 b) {
        return a.suit() == b.suit() && a.val() == 1 && b.val() == 7;
    };
    if (exp.numS() >= 2) {
        int limit = exp.numS() - 1;
        for (auto it = exp.sb(); limit > 0 && it + 1 < exp.se(); it++) {
            for (auto jt = it + 1; limit > 0 && jt < exp.se(); jt++) {
                if (ban(*it, *jt)) {
                    res.push_back(Fan::YBG1);
                    limit--;
                } else if (feng(*it, *jt)) {
                    res.push_back(Fan::XXF1);
                    limit--;
                } else if (lian(*it, *jt)) {
                    res.push_back(Fan::LL1);
                    limit--;
                } else if (lao(*it, *jt)) {
                    res.push_back(Fan::LSF1);
                    limit--;
                }
            }
        }
    }

    // Yaojiuke
    std::vector<Fan> implyYjk {
        Fan::DSX88, Fan::JLBD88, Fan::QYJ64, Fan::ZYS64, Fan::HYJ32
    };
    if(!util::common(res, implyYjk)) {
        for (auto it = exp.x34b(); it != exp.x34e(); it++) {
            bool num19 = it->isNum19();
            bool okF = it->suit() == Suit::F
                    && !util::has(res, Fan::XSX64)
                    && it->val() != info.selfWind
                    && it->val() != info.roundWind;
            if (num19 || okF)
                res.push_back(Fan::YJK1);
        }
    }

    // Ming'gang
    const std::vector<Fan> implyMg { Fan::SG88, Fan::SG32 };
    if (!util::common(res, implyMg) && exp.numO4() == 1)
        res.push_back(Fan::MG1);

    // Queyimen
    std::array<bool, 3> hasSuits;
    hasSuits[static_cast<int>(exp.pair().suit())] = true;
    for (T34 t : exp.heads())
        hasSuits[static_cast<int>(t.suit())] = true;
    int hasSuitCt = hasSuits[0] = hasSuits[1] + hasSuits[2];
    std::vector<Fan> implyQym {
        Fan::XSY64, Fan::XSX64, Fan::YSSTS48, Fan::YSSJG48, Fan::YSSBG32,
        Fan::SFK12, Fan::TBD8
    };
    if (!util::common(res, implyQym) && hasSuitCt == 2)
        res.push_back(Fan::QYM1);

    // Wuzi
    std::vector<Fan> implyWz {
        Fan::QYJ64, Fan::YSSLH64, Fan::QSK24, Fan::QYS24,
        Fan::QDA24, Fan::QZ24, Fan::QX24, Fan::SSSLH16, Fan::QDW16,
        Fan::DYW12, Fan::XYW12, Fan::DY2
    };
    if (!util::common(res, implyWz)
            && !exp.pair().isZ()
            && util::none(exp.heads(), [](T34 t) { return t.isZ(); })) {
        res.push_back(Fan::WZ1);
    }

    // Bianzhang
    // Kanzhang
    // Dandiaojiang
    HandDream wait = hand.hasDrawn() ? hand.withSpin() : HandDream::stay(hand);
    if (wait.effA().size() == 1) {
        // effA of wait-hand, not full (including drawn) hand
        switch (exp.wait()) {
        case Wait::SIDE:
            res.push_back(Fan::BZ1);
            break;
        case Wait::CLAMP:
            res.push_back(Fan::KZ1);
            break;
        case Wait::ISORIDE:
            if (!util::has(res, Fan::SG88) && !util::has(res, Fan::QQR6))
                res.push_back(Fan::DDJ1);
            break;
        default:
            break;
        }
    }

    // Zimo
    if (!util::has(res, Fan::BQR4) && !mDianpao)
        res.push_back(Fan::ZM1);
}

void FormGb::checkPick(FormGb::Fans &fs, const PointInfo &info) const
{
    if (info.duringKan)
        fs.push_back(mDianpao ? Fan::QGH8 : Fan::GSKH8);
    if (info.emptyMount) // not 'else if', addable in GB rule
        fs.push_back(mDianpao ? Fan::HDLY8 : Fan::MSHC8);
}

bool FormGb::seq3In3Or4(const Explain4 &exp, std::function<bool(T34, T34, T34)> p) const
{
    const std::array<T34, 4> &h = exp.heads();

    if (exp.numS() == 3) {
        if (p(h[0], h[1], h[2]))
            return true;
    } else if (exp.numS() == 4) {
        if (p(h[0], h[1], h[2])
                || p(h[0], h[1], h[3])
                || p(h[0], h[2], h[3])
                || p(h[1], h[2], h[3]))
            return true;
    }

    return false;
}



}


