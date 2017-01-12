#ifndef GIRLS_USUZAN_H
#define GIRLS_USUZAN_H

#include "girl.h"
#include "ticketfolder.h"



namespace saki
{



class Sawaya : public Girl
{
public:
    GIRL_CTORS(Sawaya)

    void onDice(const Table &table, TicketFolder &tickets) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    void onActivate(const Table &table, TicketFolder &tickets) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    const std::array<bool, 4> &irsRivalMask() const;
    const IrsCheckRow &irsCheckRow(int index) const override;
    int irsCheckCount() const override;

    TicketFolder forwardAction(const Table &table, Mount &mount, const Action &action) override;

    void nonMonkey(TileCount &init, Mount &mount, std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess);

    bool canUseRedCloud(unsigned &mask) const;
    bool canUseWhiteCloud(unsigned &mask) const;

    bool usingRedCloud() const;

private:
    enum Cloud
    {
        // *** SYNC order with "mClouds" ***
        BLUE, YELLOW, RED, RED_SELF, RED_RIVALS, WHITE, BLACK,
        NUM_CLOUD
    };

    enum Kamuy
    {
        // *** SYNC order with "mKamuys" ***
        PA_KOR, AT_KOR, HOYAW, HURI, PAWCI,
        NUM_KAMUY
    };

    TicketFolder handleIrsCheck(const Table &table, Mount &mount, unsigned mask);

private:
    std::array<bool, 4> mRivalMask;
    bool mPredice;
    TicketFolder mTicketsBackup;
    Who mPaKorTarget;
    bool mConsumedPaKor = false;
    bool mConsumedSecondHuri = false;

    std::array<IrsCheckRow, size_t(Cloud::NUM_CLOUD)> mClouds =
            std::array<IrsCheckRow, size_t(Cloud::NUM_CLOUD)> {
        IrsCheckRow{ false, false, "SAWAYA_C_BLUE", false, false },
        IrsCheckRow{ false, false, "SAWAYA_C_YELLOW", false, false },
        IrsCheckRow{ false, false, "SAWAYA_C_RED", true, false },
        IrsCheckRow{ true, true, "SAWAYA_C_RED_SELF", false, false },
        IrsCheckRow{ true, true, "SAWAYA_C_RED_RIVALS", false, true },
        IrsCheckRow{ false, false, "SAWAYA_C_WHITE", true, false },
        IrsCheckRow{ false, false, "SAWAYA_C_BLACK", false, false },
    };

    std::array<IrsCheckRow, size_t(Kamuy::NUM_KAMUY)> mKamuys =
            std::array<IrsCheckRow, size_t(Kamuy::NUM_KAMUY)> {
        IrsCheckRow{ false, false, "SAWAYA_K_PA_KOR", true, false },
        IrsCheckRow{ false, false, "SAWAYA_K_AT_KOR", true, false },
        IrsCheckRow{ false, false, "SAWAYA_K_HOYAW", false, false },
        IrsCheckRow{ false, false, "SAWAYA_K_HURI", true, false },
        IrsCheckRow{ false, false, "SAWAYA_K_PAWCI", false, false },
    };
};



} // namespace saki



#endif // GIRLS_USUZAN_H


