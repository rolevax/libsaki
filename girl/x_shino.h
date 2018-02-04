#ifndef SAKI_GIRL_X_SHINO_H
#define SAKI_GIRL_X_SHINO_H

#include "../table/girl.h"



namespace saki
{



class Shino : public Girl
{
public:
    GIRL_CTORS(Shino)

    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onDiscarded(const Table &table, bool spin) override;

    std::string popUpStr() const override;

private:
    void powerPinfu(const Hand &hand, const River &r, Mount &m, int posMk);
    void powerIipei(const Hand &hand, const River &r, Mount &m, int posMk, int negMk);
    bool power3sk(const Hand &hand, Mount &mount, int posMk, int negMk);
    void powerChanta(const Hand &hand, Mount &m, int mk);

private:
    T34 mBreakPair;
};



} // namespace saki



#endif // SAKI_GIRL_X_SHINO_H
