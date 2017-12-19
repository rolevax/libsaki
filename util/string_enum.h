#ifndef SAKI_STRING_ENUM_H
#define SAKI_STRING_ENUM_H

#include "../unit/action.h"
#include "../unit/meld.h"
#include "../table/table_observer.h"
#include "../form/form_gb.h"



namespace saki
{



namespace util
{



const char *stringOf(ActCode act);
ActCode actCodeOf(const char *str);

const char *stringOf(M37::Type type);

const char *stringOf(RoundResult result);
RoundResult roundResultOf(const char *str);

const char *stringOf(Fan f);



} // namespace util



} // namespace saki



#endif // SAKI_STRING_ENUM_H
