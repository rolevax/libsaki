#ifndef STRING_ENUM_H
#define STRING_ENUM_H

#include "action.h"
#include "meld.h"
#include "tableobserver.h"



namespace saki
{



const char *stringOf(ActCode act);
ActCode actCodeOf(const char *str);

const char *stringOf(M37::Type type);

const char *stringOf(RoundResult result);
RoundResult roundResultOf(const char *str);



} // namespace saki



#endif // STRING_ENUM_H


