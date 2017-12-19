#include "string_enum.h"

#include <array>
#include <string>
#include <algorithm>



namespace saki
{



namespace util
{



std::array<const char *, static_cast<int>(ActCode::NUM_ACTCODE)> actCodes
{
    "NOTHING", "PASS", "SWAP_OUT", "SPIN_OUT", "SWAP_RIICHI", "SPIN_RIICHI",
    "CHII_AS_LEFT", "CHII_AS_MIDDLE", "CHII_AS_RIGHT",
    "PON", "DAIMINKAN", "ANKAN", "KAKAN", "TSUMO", "RON",
    "RYUUKYOKU", "END_TABLE", "NEXT_ROUND", "DICE",
    "IRS_CHECK", "IRS_CLICK",
};

const char *stringOf(ActCode act)
{
    return actCodes[static_cast<int>(act)];
}

ActCode actCodeOf(const char *str)
{
    // *INDENT-OFF*
    auto eq = [str](const char *src) {
        return std::string(src) == str;
    };
    // *INDENT-ON*
    auto it = std::find_if(actCodes.begin(), actCodes.end(), eq);
    assert(it != actCodes.end());
    return static_cast<ActCode>(it - actCodes.begin());
}

std::array<const char *, static_cast<int>(ActCode::NUM_ACTCODE)> m37Types
{
    "CHII", "PON", "DAIMINKAN", "ANKAN", "KAKAN"
};

const char *stringOf(M37::Type type)
{
    return m37Types[static_cast<int>(type)];
}

std::array<const char *, static_cast<int>(RoundResult::NUM_ROUNDRES)> roundResults
{
    "TSUMO", "RON", "HP", "KSKP", "SFRT", "SKSR", "SCRC", "SCHR", "NGSMG", "ABORT"
};

const char *stringOf(RoundResult result)
{
    return roundResults[static_cast<int>(result)];
}

RoundResult roundResultOf(const char *str)
{
    // *INDENT-OFF*
    auto eq = [str](const char *src) {
        return std::string(src) == str;
    };
    // *INDENT-ON*
    auto it = std::find_if(roundResults.begin(), roundResults.end(), eq);
    assert(it != roundResults.end());
    return static_cast<RoundResult>(it - roundResults.begin());
}

std::array<const char *, static_cast<int>(Fan::NUM_FANS)> fans
{
    "Dsy88", "Dsx88", "Jlbd88", "Lqd88", "Sg88", "Lys88", "Ssy88",
    "Qyj64", "Xsy64", "Xsx64", "Zys64", "Sak64", "Ysslh64",
    "Yssts48", "Yssjg48",
    "Yssbg32", "Sg32", "Hyj32",
    "Q7d24", "Qsk24", "Qys24", "Yssts24", "Yssjg24", "Qda24", "Qz24", "Qx24",
    "Ql16", "Ssslh16", "Yssbg16", "Qdw16", "Stk16", "Sak16",
    "Dyw12", "Xyw12", "Sfk12",
    "Hl8", "Tbd8", "Sssts8", "Sssjg8", "Wfh8",
    "Mshc8", "Hdly8", "Gskh8", "Qgh8",
    "Pph6", "Hys6", "Sssbg6", "Wmq6", "Qqr6", "Sag6", "Sjk6",
    "Mag5",
    "Qdy4", "Bqr4", "Smg4", "Hjz4",
    "Jk2", "Qfk2", "Mfk2", "Mqq2", "Ph2", "Sgy2",
    "Stk2", "Sak2", "Ag2", "Dy2",
    "Ybg1", "Xxf1", "Ll1", "Lsf1", "Yjk1", "Mg1", "Qym1", "Wz1",
    "Bz1", "Kz1", "Ddj1", "Zm1"
};

const char *stringOf(Fan f)
{
    return fans[static_cast<int>(f)];
}



} // namespace util



} // namespace saki
