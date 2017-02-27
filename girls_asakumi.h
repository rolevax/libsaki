#ifndef GIRLS_ASAKUMI_H
#define GIRLS_ASAKUMI_H

#include "girl.h"



namespace saki
{



class Shino : public Girl
{
public:
    GIRL_CTORS(Shino)

    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onDiscarded(const Table &table, Who who) override;

    std::string popUpStr() const override;

private:
    void powerPinfu(const Hand &hand, const std::vector<T37> &r, Mount &m, int posMk);
    void powerIipei(const Hand &hand, const std::vector<T37> &r, Mount &m, int posMk, int negMk);
    bool power3sk(const Hand &hand, Mount &mount, int posMk, int negMk);
    void powerChanta(const Hand &hand, Mount &m, int mk);

private:
    T34 mBreakPair;
};



class Kyouka : public Girl
{
public:
    GIRL_CTORS(Kyouka)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



} // namespace saki



#endif // GIRLS_ASAKUMI_H
