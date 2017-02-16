#ifndef SAKI_GIRLS_RINKAI_H
#define SAKI_GIRLS_RINKAI_H

#include "girl.h"



namespace saki
{



class Huiyu : public Girl
{
public:
    GIRL_CTORS(Huiyu)
    static void skill(Mount &mount, const Hand &hand);
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    struct SssbgSet
    {
        int jump;
        int ofs;
        int v;
        explicit SssbgSet(bool jump, int ofs, int v) : jump(jump), ofs(ofs), v(v) { }
    };

    static const std::array<std::array<int, 3>, 6> SSSBG_OFSS;

    static void twist(Mount &mount, const TileCount &total);
    static void yssbg(Mount &mount, const TileCount &total);
    static void tbd(Mount &mount, const TileCount &total);
    static void qdqzqx(Mount &mount, const TileCount &total);
    static std::array<T34, 9> makeSssbgTars(const SssbgSet &config);
};



}



#endif // SAKI_GIRLS_RINKAI_H


