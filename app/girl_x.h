#ifndef SAKI_APP_GIRL_X_H
#define SAKI_APP_GIRL_X_H

#include "../table/girl.h"
#include "../3rd/kaguya.hpp"


namespace saki
{


class GirlX : public Girl
{
public:
    GirlX(Who who, std::string luaCode);
    GirlX(const GirlX &copy);

    std::unique_ptr<Girl> clone() const override;

    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    kaguya::State mLua;
};



} // namespace saki



#endif // SAKI_APP_GIRL_X_H
