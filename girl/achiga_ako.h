#ifndef SAKI_GIRL_ACHIGA_AKO_H
#define SAKI_GIRL_ACHIGA_AKO_H

#include "../table/girl.h"



namespace saki
{



class Ako : public Girl
{
public:
    GIRL_CTORS(Ako)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    static int sskDist(const TileCount &closed, T34 head);
    static int ittDist(const TileCount &closed, T34 head);
    static void oneDragTwo(Mount &mount, const TileCount &closed, T34 head);
    static void thinFill(Mount &mount, const TileCount &closed, T34 heads);
};



} // namespace saki



#endif // SAKI_GIRL_ACHIGA_AKO_H
