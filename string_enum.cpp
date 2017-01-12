#include "string_enum.h"

#include <array>
#include <string>
#include <algorithm>



namespace saki
{



static std::array<const char*, static_cast<int>(ActCode::NUM_ACTCODE)> actCodes
{
    "NOTHING", "PASS", "SWAP_OUT", "SPIN_OUT",
    "CHII_AS_LEFT", "CHII_AS_MIDDLE", "CHII_AS_RIGHT",
    "PON", "DAIMINKAN", "ANKAN", "KAKAN", "RIICHI", "TSUMO",
    "RON", "RYUUKYOKU", "END_TABLE", "NEXT_ROUND", "DICE",
    "IRS_CHECK", "IRS_CLICK", "IRS_RIVAL"
};

const char *stringOf(ActCode act)
{
    return actCodes[static_cast<int>(act)];
}

ActCode actCodeOf(const char *str)
{
    auto it = std::find_if(actCodes.begin(), actCodes.end(), [str](const char *src){
        return std::string(src) == str;
    });
    assert(it != actCodes.end());
    return static_cast<ActCode>(it - actCodes.begin());
}

static std::array<const char*, static_cast<int>(ActCode::NUM_ACTCODE)> m37Types
{
    "CHII", "PON", "DAIMINKAN", "ANKAN", "KAKAN"
};

const char *stringOf(M37::Type type)
{
    return m37Types[static_cast<int>(type)];
}

static std::array<const char*, static_cast<int>(RoundResult::NUM_ROUNDRES)> roundResults
{
    "TSUMO", "RON", "HP", "KSKP", "SFRT", "SKSR", "SCRC", "SCHR", "NGSMG", "ABORT"
};

const char *stringOf(RoundResult result)
{
    return roundResults[static_cast<int>(result)];
}

RoundResult roundResultOf(const char *str)
{
    auto it = std::find_if(roundResults.begin(), roundResults.end(), [str](const char *src){
        return std::string(src) == str;
    });
    assert(it != roundResults.end());
    return static_cast<RoundResult>(it - roundResults.begin());
}



} // namespace saki


