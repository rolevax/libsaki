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
const int round = 0;
const int extra = 0;
const int dealer = 1;
const bool allLast = false;
const int deposit = 0;
const uint32_t state = 70859629;
#endif



#ifdef LIBSAKI_CHEAT_PRINCESS
using namespace tiles37;

const std::array<std::vector<T37>, 4> inits = {
    std::vector<T37> { 1_m, 2_m, 3_m, 4_m, 5_m, 6_m, 7_m, 8_m, 1_f, 1_f, 2_p, 3_p },
    std::vector<T37> {  },
    std::vector<T37> {  },
    std::vector<T37> {  }
};

const std::vector<T37> wall = {
    9_m, 1_y, 1_y, 1_y, 1_p, 1_p, 1_p, 1_p
};

const std::vector<T37> dead = {
};
#endif



} // namespace cheat



} // namespace saki



#endif // SAKI_DEBUG_CHEAT_H


