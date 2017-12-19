#ifndef SAKI_GIRLS_USUZAN_H
#define SAKI_GIRLS_USUZAN_H

#include "girl.h"



namespace saki
{



class Sawaya : public Girl
{
public:
    GIRL_CTORS(Sawaya)

    void onDice(util::Rand &rand, const Table &table, Choices &choices) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    void onActivate(const Table &table, Choices &choices) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    const IrsCheckRow &irsCheckRow(int index) const override;
    int irsCheckCount() const override;

    Choices forwardAction(const Table &table, Mount &mount, const Action &action) override;

    void nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;

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
        PA_KOR, PA_KOR_R, PA_KOR_C, PA_KOR_L, AT_KOR, HOYAW, HURI, PAWCI,
        NUM_KAMUY
    };

    Choices handleIrsCheck(const Table &table, Mount &mount, unsigned mask);

private:
    std::array<bool, 4> mRivalMask;
    bool mPredice;
    Choices mChoicesBackup;
    Who mPaKorTarget;
    bool mConsumedPaKor = false;
    bool mConsumedSecondHuri = false;

    std::array<IrsCheckRow, size_t(Cloud::NUM_CLOUD)> mClouds =
        std::array<IrsCheckRow, size_t(Cloud::NUM_CLOUD)> {
        IrsCheckRow { false, false, "SAWAYA_C_BLUE", false, false },
        IrsCheckRow { false, false, "SAWAYA_C_YELLOW", false, false },
        IrsCheckRow { false, false, "SAWAYA_C_RED", true, false },
        IrsCheckRow { true, true, "SAWAYA_C_RED_SELF", false, true },
        IrsCheckRow { true, true, "SAWAYA_C_RED_RIVALS", false, false },
        IrsCheckRow { false, false, "SAWAYA_C_WHITE", true, false },
        IrsCheckRow { false, false, "SAWAYA_C_BLACK", false, false },
    };

    std::array<IrsCheckRow, size_t(Kamuy::NUM_KAMUY)> mKamuys =
        std::array<IrsCheckRow, size_t(Kamuy::NUM_KAMUY)> {
        IrsCheckRow { false, false, "SAWAYA_K_PA_KOR", true, false },
        IrsCheckRow { true, true, "SAWAYA_K_PA_KOR_R", false, true },
        IrsCheckRow { true, true, "SAWAYA_K_PA_KOR_C", false, false },
        IrsCheckRow { true, true, "SAWAYA_K_PA_KOR_L", false, false },
        IrsCheckRow { false, false, "SAWAYA_K_AT_KOR", true, false },
        IrsCheckRow { false, false, "SAWAYA_K_HOYAW", false, false },
        IrsCheckRow { false, false, "SAWAYA_K_HURI", true, false },
        IrsCheckRow { false, false, "SAWAYA_K_PAWCI", false, false },
    };
};



} // namespace saki



#endif // SAKI_GIRLS_USUZAN_H
