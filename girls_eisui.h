#ifndef SAKI_GIRLS_EISUI_H
#define SAKI_GIRLS_EISUI_H

#include "girl.h"



namespace saki
{



class Hatsumi : public Girl
{
public:
    GIRL_CTORS(Hatsumi)

    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void nonMonkey(Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;
};

class Kasumi : public Girl
{
public:
    GIRL_CTORS(Kasumi)

    void onDice(Rand &rand, const Table &table, Choices &choices) override;
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    Choices forwardAction(const Table &table, Mount &mount, const Action &action) override;
    const IrsCheckRow &irsCheckRow(int index) const override;
    int irsCheckCount() const override;

    void nonMonkey(Rand &rand, TileCount &init, Mount &mount,
                   std::bitset<NUM_NM_SKILL> &presence,
                   const Princess &princess) override;

private:
    Choices mChoicesBackup;
    IrsCheckRow mZim { false, false, "KASUMI_ZIM", true, false };
    Suit mZimSuit;
};



} // namespace saki



#endif // SAKI_GIRLS_EISUI_H


