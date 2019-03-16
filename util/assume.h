#ifndef SAKI_ASSUME_H
#define SAKI_ASSUME_H

#include <cassert>

// *INDENT-OFF*

#ifndef NDEBUG
#define assume_opt_out(cond) do { if (!(cond)) __builtin_unreachable(); } while (false)
#else
#define assume_opt_out(cond) do {} while (false)
#endif

#define unreached() do { std::abort(); __builtin_unreachable(); } while (false)

// *INDENT-ON*

#endif // SAKI_ASSUME_H
