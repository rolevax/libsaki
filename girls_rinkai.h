#ifndef SAKI_GIRLS_RINKAI_H
#define SAKI_GIRLS_RINKAI_H

#include "girl.h"
#include "pointinfo.h"

#include <bitset>



namespace saki
{



class Huiyu : public Girl
{
public:
    GIRL_CTORS(Huiyu)
    static void skill(Mount &mount, const Hand &hand, const PointInfo &info);
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    struct SssbgSet
    {
        int jump;
        int ofs;
        int v;
        explicit SssbgSet(bool jump, int ofs, int v) : jump(jump), ofs(ofs), v(v) { }
    };

    static bool expand(Mount &mount, const TileCount &total);
    static int yssbg(std::bitset<34> &reqs, const TileCount &total);
    static int colors(std::bitset<34> &reqs, const TileCount &total);
    static int sssjg(std::bitset<34> &reqs, const TileCount &total);
    static int tbd(std::bitset<34> &reqs, const TileCount &total);
    static int qdqzqx(std::bitset<34> &reqs, const TileCount &total);
    static int gtlt5(std::bitset<34> &reqs, const TileCount &total);
    static std::array<T34, 9> makeSssbgTars(const SssbgSet &config);
};



}



#endif // SAKI_GIRLS_RINKAI_H


