#ifndef SAKI_DEBUG_CHEAT_H
#define SAKI_DEBUG_CHEAT_H

#include "tile.h"

#include <cstdint>



///
/// uncomment cheat flags to enable debug-cheat build
///
//#define LIBSAKI_CHEAT_ROUND
//#define LIBSAKI_CHEAT_PRINCESS
//#define LIBSAKI_CHEAT_AI

#ifdef LIBSAKI_CHEAT_ROUND
#ifdef NDEBUG
static_assert(false, "cheat in release mode");
#endif
#endif

#ifdef LIBSAKI_CHEAT_PRINCESS
#ifdef NDEBUG
static_assert(false, "cheat in release mode");
#endif
#endif

#ifdef LIBSAKI_CHEAT_AI
#ifdef NDEBUG
static_assert(false, "cheat in release mode");
#endif
#endif



namespace saki
{



namespace cheat
{



#ifdef LIBSAKI_CHEAT_ROUND
const int round = 1;
const int extra = 0;
const int dealer = 3;
const bool allLast = false;
const int deposit = 0;
const uint32_t state = 1291430400;
#endif



#ifdef LIBSAKI_CHEAT_PRINCESS
using namespace tiles37;

const std::array<std::vector<T37>, 4> inits = {
    std::vector<T37> {  },
    std::vector<T37> { 1_m, 2_m, 3_m, 7_m, 8_m, 9_m, 2_p,  3_p, 4_p, 7_p, 8_p, 1_f, 1_f  },
    std::vector<T37> {  },
    std::vector<T37> {  }
};

const std::vector<T37> wall = {
};

const std::vector<T37> dead = {
};
#endif



} // namespace cheat



} // namespace saki



#endif // SAKI_DEBUG_CHEAT_H


