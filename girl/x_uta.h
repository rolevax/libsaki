#ifndef SAKI_GIRL_X_UTA_H
#define SAKI_GIRL_X_UTA_H

#include "../table/girl.h"



namespace saki
{



class Uta : public Girl
{
public:
    GIRL_CTORS(Uta)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    void power3sk(const Hand &hand, const River &river, Mount &mount);
    bool tryPowerDye(const Hand &hand, Mount &mount);
};



} // namespace saki



#endif // SAKI_GIRL_X_UTA_H
