#ifndef SAKI_GIRL_EISUI_HATSUMI_H
#define SAKI_GIRL_EISUI_HATSUMI_H

#include "../table/princess.h"



namespace saki
{



class Hatsumi : public Girl, public HrhBargainer
{
public:
    GIRL_CTORS(Hatsumi)

    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    std::optional<HrhInitFix> onHrhRaid(const Table &table) override;

    HrhBargainer *onHrhBargain(const Table &table) override;
    Claim hrhBargainClaim(int plan, T34 t) override;
    int hrhBargainPlanCt() override;
    void onHrhBargained(int plan, Mount &mount) override;
};



} // namespace saki



#endif // SAKI_GIRL_EISUI_HATSUMI_H
