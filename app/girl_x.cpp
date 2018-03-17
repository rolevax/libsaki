#include "girl_x.h"
#include "../3rd/kaguya.hpp"



namespace saki
{



GirlX::GirlX(Who who)
    : Girl(who, Girl::Id::CUSTOM)
{
    kaguya::State state;
    state.dostring("a = 1");
}

void GirlX::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;
    (void) mount;
    (void) who;
    (void) rinshan;
}



} // namespace saki
