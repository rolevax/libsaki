#ifndef SAKI_GIRLS_USUZAN_H
#define SAKI_GIRLS_USUZAN_H

#include "../table/irs_chance.h"



namespace saki
{



class Sawaya : public Girl
{
public:
    GIRL_CTORS(Sawaya)

    void onDice(util::Rand &rand, const Table &table) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onIrsChecked(const Table &table, Mount &mount) override;

    void nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;

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
    std::array<bool, 4> mRivalMask;
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



#endif // SAKI_GIRLS_USUZAN_H
