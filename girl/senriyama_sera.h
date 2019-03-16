#ifndef SAKI_GIRL_SENRIYAMA_SERA_H
#define SAKI_GIRL_SENRIYAMA_SERA_H

#include "../table/girl.h"



namespace saki
{



///
/// "Sera" is more commonly used than "Seira".
/// "Se-ra" is better, and say fuck to ascii and identifier syntax.
///
class Sera : public GirlCrtp<Sera>
{
public:
    using GirlCrtp<Sera>::GirlCrtp;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



} // namespace saki



#endif // SAKI_GIRL_SENRIYAMA_SERA_H
