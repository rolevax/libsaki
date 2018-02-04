#ifndef SAKI_GIRL_RINKAI_HUIYU_H
#define SAKI_GIRL_RINKAI_HUIYU_H

#include "../table/girl.h"
#include "../form/form_ctx.h"

#include <bitset>



namespace saki
{



class Huiyu : public Girl
{
public:
    GIRL_CTORS(Huiyu)

    static void skill(Mount &mount, const Hand &hand, const FormCtx &ctx);

    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

    ChoiceFilter filterChoice(const Table &table, Who who) override;

private:
    struct SssbgSet
    {
        int jump;
        int ofs;
        int v;
        explicit SssbgSet(bool jump, int ofs, int v) : jump(jump), ofs(ofs), v(v) {}
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



} // namespace saki



#endif // SAKI_GIRL_RINKAI_HUIYU_H
