#include "skillpop.h"
#include "tableobserver.h"

#include <cassert>



namespace saki
{



SkillToken tokenOf(const T37 &t)
{
    if (t.isAka5())
        return SkillToken(int(SkillToken::M0) + int(t.suit()));
    else
        return SkillToken(int(SkillToken::M1) + t.id34());
}

T37 tileOf(SkillToken t)
{
    int i = static_cast<int>(t);
    int m0 = static_cast<int>(SkillToken::M0);
    if (i >= m0)
        return T37(Suit(i - m0), 0);
    else
        return T37(i - static_cast<int>(SkillToken::M1));
}

SkillToken tokenOf(Who who)
{
    return SkillToken(int(SkillToken::PLAYER0) + who.index());
}

Who whoOf(SkillToken t)
{
    return Who(int(t) - int(SkillToken::PLAYER0));
}

#define ACT_TO_SKILL(code) case ActCode:: code : return SkillToken:: code

SkillToken tokenOf(ActCode act)
{
    switch (act) {
    case ActCode::CHII_AS_LEFT:
    case ActCode::CHII_AS_MIDDLE:
    case ActCode::CHII_AS_RIGHT:
        return SkillToken::CHII;
    ACT_TO_SKILL(PON);
    ACT_TO_SKILL(DAIMINKAN);
    ACT_TO_SKILL(ANKAN);
    ACT_TO_SKILL(KAKAN);
    ACT_TO_SKILL(RIICHI);
    ACT_TO_SKILL(TSUMO);
    ACT_TO_SKILL(RON);
    default:
        unreached("tokenOfAct() unhandled act");
        throw "unreached";
    }
}

#define SKILL_TO_ACT(code) case SkillToken:: code : return ActCode:: code

ActCode actOf(SkillToken t)
{
    switch (t) {
    case SkillToken::CHII:
        return ActCode::CHII_AS_LEFT;
    SKILL_TO_ACT(PON);
    SKILL_TO_ACT(DAIMINKAN);
    SKILL_TO_ACT(ANKAN);
    SKILL_TO_ACT(KAKAN);
    SKILL_TO_ACT(RIICHI);
    SKILL_TO_ACT(TSUMO);
    SKILL_TO_ACT(RON);
    default:
        unreached("actOfToken() unhandled act");
        throw "unreached";
    }
}

#define BARK_TO_SKILL(code) case M37::Type:: code : return SkillToken:: code

SkillToken tokenOf(M37::Type type)
{
    switch (type) {
    BARK_TO_SKILL(CHII);
    BARK_TO_SKILL(PON);
    BARK_TO_SKILL(DAIMINKAN);
    BARK_TO_SKILL(ANKAN);
    BARK_TO_SKILL(KAKAN);
    default:
        unreached("tokenOf(M37::Type) unhandled type");
        throw "unreached";
    }
}

#define SKILL_TO_BARK(code) case SkillToken:: code : return M37::Type:: code

M37::Type barkTypeOf(SkillToken t)
{
    switch (t) {
    SKILL_TO_BARK(CHII);
    SKILL_TO_BARK(PON);
    SKILL_TO_BARK(DAIMINKAN);
    SKILL_TO_BARK(ANKAN);
    SKILL_TO_BARK(KAKAN);
    default:
        unreached("barkTypeOf() unhandled type");
        throw "unreached";
    }
}

#define RESULT_TO_SKILL(code) case RoundResult:: code : return SkillToken:: code

SkillToken tokenOf(RoundResult result)
{
    switch (result) {
    RESULT_TO_SKILL(TSUMO);
    RESULT_TO_SKILL(RON);
    RESULT_TO_SKILL(HP);
    RESULT_TO_SKILL(KSKP);
    RESULT_TO_SKILL(SFRT);
    RESULT_TO_SKILL(SKSR);
    RESULT_TO_SKILL(SCRC);
    RESULT_TO_SKILL(SCHR);
    RESULT_TO_SKILL(NGSMG);
    default:
        unreached("tokenOfRyuu() unhandled act");
        throw "unreached";
    }
}

#define SKILL_TO_RESULT(code) case SkillToken:: code : return RoundResult:: code

RoundResult resultOf(SkillToken t)
{
    switch (t) {
    SKILL_TO_RESULT(TSUMO);
    SKILL_TO_RESULT(RON);
    SKILL_TO_RESULT(HP);
    SKILL_TO_RESULT(KSKP);
    SKILL_TO_RESULT(SFRT);
    SKILL_TO_RESULT(SKSR);
    SKILL_TO_RESULT(SCRC);
    SKILL_TO_RESULT(SCHR);
    SKILL_TO_RESULT(NGSMG);
    default:
        unreached("ryuuOfToken() unhandled act");
        throw "unreached";
    }
}


bool tokenIsTile(SkillToken t)
{
    return SkillToken::M1 <= t && t <= SkillToken::S0;
}



} // namespace saki


