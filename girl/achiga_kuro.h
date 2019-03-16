#ifndef SAKI_GIRL_ACHIGA_KURO_H
#define SAKI_GIRL_ACHIGA_KURO_H

#include "../table/princess.h"



namespace saki
{



class Kuro : public GirlCrtp<Kuro>, public HrhBargainer
{
public:
    using GirlCrtp<Kuro>::GirlCrtp;
    void onMonkey(std::array<Exist, 4> &exists, const Table &table) override;
    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    void onTableEvent(const Table &table, const TableEvent &event) override;

    HrhBargainer *onHrhBargain(const Table &table) override;
    Claim hrhBargainClaim(int plan, T34 t) override;
    int hrhBargainPlanCt() override;
    void onHrhBargained(int plan, Mount &mount) override;

private:
    static const int EJECT_MK = 5000;
    bool mCd = false;
};



} // namespace saki



#endif // SAKI_GIRL_ACHIGA_KURO_H
