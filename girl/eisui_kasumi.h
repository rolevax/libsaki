#ifndef SAKI_GIRL_EISUI_KASUMI_H
#define SAKI_GIRL_EISUI_KASUMI_H

#include "../table/princess.h"
#include "../table/irs_ctrl.h"



namespace saki
{



class Kasumi : public Girl, public HrhBargainer
{
public:
    GIRL_CTORS(Kasumi)

    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onMonkey(std::array<Exist, 4> &exists, const Table &table) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onIrsChecked(const Table &table, Mount &mount) override;

    HrhBargainer *onHrhBargain() override;
    Claim hrhBargainClaim(int plan, T34 t) override;
    int hrhBargainPlanCt() override;
    void onHrhBargained(int plan, Mount &mount) override;

protected:
    IrsCtrlGetter attachIrsOnDice() override;

private:
    IrsCtrlCheck<Kasumi> mIrsCtrl {
        Choices::ModeIrsCheck {
            "KASUMI",
            IrsCheckList {
                IrsCheckItem::CHECK_ENABLED
            }
        }
    };
    Suit mZimSuit;
};



} // namespace saki



#endif // SAKI_GIRL_EISUI_KASUMI_H
