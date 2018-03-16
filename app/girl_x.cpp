#include "girl_x.h"

namespace saki
{



GirlX::GirlX(Who who)
    : Girl(who, Girl::Id::CUSTOM)
{
}

void GirlX::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;
    (void) mount;
    (void) who;
    (void) rinshan;
}



} // namespace saki
