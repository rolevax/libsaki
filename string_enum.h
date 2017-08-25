#ifndef SAKI_STRING_ENUM_H
#define SAKI_STRING_ENUM_H

#include "action.h"
#include "meld.h"
#include "table_observer.h"
#include "form_gb.h"



namespace saki
{



const char *stringOf(ActCode act);
ActCode actCodeOf(const char *str);

const char *stringOf(M37::Type type);

const char *stringOf(RoundResult result);
RoundResult roundResultOf(const char *str);

const char *stringOf(Fan f);



} // namespace saki



#endif // SAKI_STRING_ENUM_H


