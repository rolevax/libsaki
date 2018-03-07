#ifndef SAKI_MOUNT_H
#define SAKI_MOUNT_H

#include "../form/tile_count.h"
#include "../util/rand.h"

#include <memory>
#include <array>
#include <vector>
#include <list>



namespace saki
{



class Exist
{
public:
    static const int BASE_MK = 10;

    struct Polar
    {
        struct Cy
        {
            explicit Cy(const T37 &t, int e) : t(t), e(e) {}
            T37 t;
            int e;
        };

        std::vector<Cy> pos;
        std::vector<Cy> npos;
    };

    Exist();
    Exist(const Exist &copy) = default;
    Exist &operator=(const Exist &assign) = default;

    void inc(const T37 &t, int delta);
    void inc(T34 t, int delta);
    void addBaseMk(const TileCount &stoch);

    Polar polarize(const TileCount &stoch) const;

private:
    std::array<int, 34> mBlack;
    std::array<int, 3> mRed;
};



class MountPrivate
{
protected:
    MountPrivate(TileCount::AkadoraCount fillMode);
    MountPrivate(const MountPrivate &copy) = default;
    MountPrivate &operator=(const MountPrivate &assign) = default;

protected:
    util::Stactor<T37, 5> mDrids;
    util::Stactor<T37, 5> mUrids;
    int mKanCt = 0;
    int mRemain = 136 - 14;

    TileCount mStochA;
    TileCount mStochB;
};

class Mount : private MountPrivate
{
public:
    enum Exit { PII, RINSHAN, DORAHYOU, URAHYOU, NUM_EXITS };

    explicit Mount(TileCount::AkadoraCount fillMode);
    explicit Mount(const Mount &copy);
    Mount &operator=(const Mount &assign);

    void initFill(util::Rand &rand, TileCount &init, Exist &exist);
    const T37 &initPopExact(const T37 &t);
    T37 pop(util::Rand &rand, bool rinshan = false);

    int remainPii() const;
    int remainRinshan() const;
    int remainA(T34 t) const;
    int remainA(const T37 &t) const;
    bool affordA(const TileCount &need) const;

    const util::Stactor<T37, 5> &getDrids() const;
    const util::Stactor<T37, 5> &getUrids() const;

    void lightA(T34 t, int delta, bool rinshan = false);
    void lightA(const T37 &t, int delta, bool rinshan = false);
    void lightB(T34 t, int delta, bool rinshan = false);
    void lightB(const T37 &t, int delta, bool rinshan = false);
    void incMk(Exit exit, size_t pos, T34 t, int delta, bool bSpace);
    void incMk(Exit exit, size_t pos, const T37 &t, int delta, bool bSpace);
    void collapse(Exit exit, std::size_t pos, const T37 &t);
    void loadB(const T37 &t, int count);

    void flipIndic(util::Rand &rand);
    void digIndic(util::Rand &rand);

private:
    struct Erwin
    {
        Exist exA;
        Exist exB;
        T37 tile;
        bool earlyCollapse;
        Erwin() : earlyCollapse(false) {}
        Erwin(const T37 &t) : tile(t), earlyCollapse(true) {}
    };


    using ErwinQueue = std::list<std::unique_ptr<Erwin>>;

    const std::unique_ptr<Erwin> &prepareSuperpos(Exit exit, std::size_t pos);
    T37 popFrom(util::Rand &rand, Exit exit);
    std::vector<T37> popExist(util::Rand &rand, Exist &exist, int need);
    T37 popExist(util::Rand &rand, Exist &exA, Exist &exB);
    std::vector<T37> popPolar(util::Rand &rand, Exist::Polar &polar, TileCount &stoch, int need);
    T37 popScientific(util::Rand &rand);

private:
    // *** SYNC with copy constructor and assign operator ***
    std::array<ErwinQueue, NUM_EXITS> mErwinQueues;
};

int operator%(const util::Stactor<T37, 5> &indics, T34 t);



} // namespace saki



#endif // SAKI_MOUNT_H
