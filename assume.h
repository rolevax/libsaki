#ifndef ASSUME_H
#define ASSUME_H

#include <cassert>

#ifndef NDEBUG
#define assume_opt_out(cond) do { if (!(cond)) __builtin_unreachable(); } while (false)
#else
#define assume_opt_out(cond) do { } while (false)
#endif

#define unreached(str) do { assert(false && str); __builtin_unreachable(); } while (false)

#endif // ASSUME_H

