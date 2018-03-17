#ifndef SAKI_PRINCESS_H
#define SAKI_PRINCESS_H

#include "table.h"



namespace saki
{



///
/// \brief Data object for directly assigning init-hand
///
struct HrhInitFix
{
public:
    enum class Priority { HIGH, LOW, NONE };

    struct LoadArgs
    {
        LoadArgs(T37 t, int c) : tile(t), ct(c) {}
        T37 tile;
        int ct;
    };

    Priority priority = Priority::NONE;
    util::Stactor<T37, 13> targets;
    util::Stactor<LoadArgs, 8> loads;
};



///
/// \brief Interface for working with the 'bargain' stage
///
class HrhBargainer
{
public:
    ///
    /// NONE: don't care
    /// ANY: nobody shall claim this tile
    /// ALL: claim all this tile except raided ones
    /// FOUR: claim all four of this tile
    ///
    enum class Claim { NONE, ANY, ALL, FOUR };

    virtual ~HrhBargainer() = default;
    virtual Claim hrhBargainClaim(int plan, T34 t) = 0;
    virtual int hrhBargainPlanCt() = 0;
    virtual void onHrhBargained(int plan, Mount &mount) = 0;
};



class Princess
{
public:
    explicit Princess(const Table &table, util::Rand &rand, Mount &mount,
                      const std::array<std::unique_ptr<Girl>, 4> &girls);

    Princess(const Princess &copy) = delete;
    Princess &operator=(const Princess &assign) = delete;

    std::array<Hand, 4> dealAndFlip();

private:
    void debugCheat(std::array<TileCount, 4> &res);
    void raid(std::array<TileCount, 4> &inits);
    void bargain();
    void beg(std::array<TileCount, 4> &inits);
    util::Stactor<int, 34> permutation(int size);
    std::array<Hand, 4> monkey(std::array<TileCount, 4> &nonMonkeys);
    void fixInit(TileCount &init, const HrhInitFix &fix);

private:
    class BargainResult
    {
    public:
        int plan() const;
        HrhBargainer *bargainer() const;
        void set(HrhBargainer *b, int p);
        bool active() const;

    private:
        int mPlan = -1;
        HrhBargainer *mBargainer = nullptr;
    };

    const Table &mTable;
    util::Rand &mRand;
    Mount &mMount;
    const std::array<std::unique_ptr<Girl>, 4> &mGirls;
    std::array<BargainResult, 4> mBargainResults;
};



} // namespace saki



#endif // SAKI_PRINCESS_H
