#ifndef SAKI_GIRL_EISUI_KASUMI_H
#define SAKI_GIRL_EISUI_KASUMI_H

#include "../table/irs_ctrl.h"



namespace saki
{



class Kasumi : public Girl
{
public:
    GIRL_CTORS(Kasumi)

    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onIrsChecked(const Table &table, Mount &mount) override;

    void nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;

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
