#ifndef SKILLPOP_H
#define SKILLPOP_H

#include "tile.h"
#include "action.h"
#include "meld.h"

#include <vector>
#include <string>



namespace saki
{



enum class RoundResult;

enum class SkillToken
{
    DRAW, TMKR, TKR, CHII, PON, DAIMINKAN,
    ANKAN, KAKAN, RIICHI, TSUMO, RON,
    KANDORA_I, URADORA_I,
    HP, KSKP, SFRT, SKSR, SCRC, SCHR, NGSMG,
    PLAYER0, PLAYER1, PLAYER2, PLAYER3,
    M1, M2, M3, M4, M5, M6, M7, M8, M9,
    P1, P2, P3, P4, P5, P6, P7, P8, P9,
    S1, S2, S3, S4, S5, S6, S7, S8, S9,
    F1, F2, F3, F4, Y1, Y2, Y3,
    M0, P0, S0,
    EMOJI_OO, DUANG, GREEN
};

using SkillExpr = std::vector<SkillToken>;

bool tokenIsTile(SkillToken t);

SkillToken tokenOf(const T37 &t);
T37 tileOf(SkillToken t);

SkillToken tokenOf(Who who);
Who whoOf(SkillToken t);

SkillToken tokenOf(ActCode act);
ActCode actOf(SkillToken t);

SkillToken tokenOf(M37::Type type);
M37::Type barkTypeOf(SkillToken t);

SkillToken tokenOf(RoundResult result);
RoundResult resultOf(SkillToken t);



} // namespace saki



#endif // SKILLPOP_H


