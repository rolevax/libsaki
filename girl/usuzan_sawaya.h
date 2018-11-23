#ifndef SAKI_GIRL_USUZAN_SAWAYA_H
#define SAKI_GIRL_USUZAN_SAWAYA_H

#include "../table/princess.h"
#include "../table/irs_ctrl.h"



namespace saki
{



class Sawaya : public Girl, public HrhBargainer
{
public:
    GIRL_CTORS(Sawaya)

    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onDice(util::Rand &rand, const Table &table) override;
    void onMonkey(std::array<Exist, 4> &exists, const Table &table) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onIrsChecked(const Table &table, Mount &mount) override;

    HrhBargainer *onHrhBargain(const Table &table) override;
    Claim hrhBargainClaim(int plan, T34 t) override;
    int hrhBargainPlanCt() override;
    void onHrhBargained(int plan, Mount &mount) override;

    bool canUseRedCloud(unsigned &mask) const;
    bool canUseWhiteCloud(unsigned &mask) const;

    bool usingRedCloud() const;

protected:
    IrsCtrlGetter attachIrsOnDice() override;
    IrsCtrlGetter attachIrsOnDrawn(const Table &table) override;

private:
    enum Cloud
    {
        // *** SYNC order with "mCloudCtrl" ***
        BLUE, YELLOW, RED, RED_SELF, RED_RIVALS, WHITE, BLACK,
        NUM_CLOUD
    };

    enum Kamuy
    {
        // *** SYNC order with "mKamuyCtrl" ***
        PA_KOR, PA_KOR_R, PA_KOR_C, PA_KOR_L, AT_KOR, HOYAW, HURI, PAWCI,
        NUM_KAMUY
    };

    bool usingCloud(Cloud which) const;
    bool usingKamuy(Kamuy which) const;

private:
    bool mPredice;
    Who mPaKorTarget;
    bool mConsumedPaKor = false;
    bool mConsumedSecondHuri = false;

    IrsCtrlCheck<Sawaya> mCloudCtrl {
        Choices::ModeIrsCheck {
            "SAWAYA_CLOUD",
            IrsCheckList {
                IrsCheckItem::CHECK_DISABLED,
                IrsCheckItem::CHECK_DISABLED,
                IrsCheckItem::CHECK_ENABLED,
                IrsCheckItem::CHILD_RADIO_DEFAULT,
                IrsCheckItem::CHILD_RADIO,
                IrsCheckItem::CHECK_ENABLED,
                IrsCheckItem::CHECK_DISABLED,
            }
        }
    };

    IrsCtrlClickCheck<Sawaya> mKamuyCtrl {
        Choices::ModeIrsCheck {
            "SAWAYA_KAMUY",
            IrsCheckList {
                IrsCheckItem::CHECK_ENABLED,
                IrsCheckItem::CHILD_RADIO_DEFAULT,
                IrsCheckItem::CHILD_RADIO,
                IrsCheckItem::CHILD_RADIO,
                IrsCheckItem::CHECK_ENABLED,
                IrsCheckItem::CHECK_DISABLED,
                IrsCheckItem::CHECK_ENABLED,
                IrsCheckItem::CHECK_DISABLED,
            }
        },
        &Sawaya::mKamuyCtrl
    };
};



} // namespace saki



#endif // SAKI_GIRL_USUZAN_SAWAYA_H
