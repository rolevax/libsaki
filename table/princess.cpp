#include "princess.h"
#include "../util/rand.h"
#include "../util/debug_cheat.h"

#include <algorithm>



namespace saki
{



Princess::Princess(const Table &table, util::Rand &rand, Mount &mount,
                   const std::array<std::unique_ptr<Girl>, 4> &girls)
    : mTable(table)
    , mRand(rand)
    , mMount(mount)
    , mGirls(girls)
{
}

///
/// \brief Deal and flip the first dora indicator
///
std::array<Hand, 4> Princess::dealAndFlip()
{
    std::array<TileCount, 4> inits;

#ifdef LIBSAKI_CHEAT_PRINCESS
    debugCheat(inits);
    mMount.flipIndic(mRand);
#else
    raid(inits);
    bargain();
    mMount.flipIndic(mRand);
    beg(inits);
#endif

    return monkey(inits);
}

void Princess::debugCheat(std::array<TileCount, 4> &res)
{
#ifdef LIBSAKI_CHEAT_PRINCESS
    for (size_t pos = 0; pos < cheat::wall.size(); pos++)
        mMount.collapse(Mount::PII, pos, cheat::wall[pos]);

    for (size_t pos = 0; pos < cheat::dead.size(); pos++)
        mMount.collapse(Mount::RINSHAN, pos, cheat::dead[pos]);

    for (int w = 0; w < 4; w++)
        for (const T37 &t : cheat::inits[w])
            res[w].inc(mMount.initPopExact(t), 1);

#else
    (void) res;
#endif
}

void Princess::raid(std::array<TileCount, 4> &inits)
{
    std::array<std::optional<HrhInitFix>, 4> fixes;
    for (int w = 0; w < 4; w++)
        fixes[w] = mGirls[w]->onHrhRaid(mTable);

    using Pri = HrhInitFix::Priority;
    for (Pri pri : { Pri::HIGH, Pri::LOW }) {
        for (int w = 0; w < 4; w++) {
            const auto &opt = fixes[w];
            if (opt.has_value() && opt->priority == pri) {
                fixInit(inits[w], *opt);
            }
        }
    }
}

void Princess::bargain()
{
    util::Stactor<HrhBargainer *, 4> bargainers;
    util::Stactor<Who, 4> whos;
    int totalPlanCt = 1;
    for (int w = 0; w < 4; w++) {
        auto bargainer = mGirls[w]->onHrhBargain(mTable);
        if (bargainer != nullptr) {
            assert(bargainer->hrhBargainPlanCt() > 0);
            totalPlanCt *= bargainer->hrhBargainPlanCt();
            bargainers.pushBack(bargainer);
            whos.pushBack(Who(w));
        }
    }

    util::Stactor<util::Stactor<int, 34>, 4> perms;
    for (HrhBargainer *bar : bargainers)
        perms.emplaceBack(permutation(bar->hrhBargainPlanCt()));

    // *INDENT-OFF*
    auto getCurrPlans = [&perms](int bigPlan) {
        util::Stactor<int, 4> currPlans;

        for (const auto &perm : perms) {
            int planSize = perm.size();
            int index = bigPlan % planSize;
            bigPlan /= planSize;
            currPlans.pushBack(perm[index]);
        }

        return currPlans;
    };

    auto checkPlans = [&](const util::Stactor<int, 4> &plans) {
        for (T34 t : tiles34::ALL34) {
            using Claim = HrhBargainer::Claim;
            util::Stactor<Claim, 4> claims;
            for (size_t i = 0; i < bargainers.size(); i++)
                claims.pushBack(bargainers[i]->hrhBargainClaim(plans[i], t));

            int any = std::count(claims.begin(), claims.end(), Claim::ANY);
            int all = std::count(claims.begin(), claims.end(), Claim::ALL);
            int four = std::count(claims.begin(), claims.end(), Claim::FOUR);

            // claimer must be unique
            if (all + four > 1)
                return false;

            // some disclaimers disclaim the claimer
            if (all + four == 1 && any > 0)
                return false;

            // at least one for each
            if (mMount.remainA(t) < any)
                return false;

            if (four == 1 && mMount.remainA(t) != 4)
                return false;
        }

        return true;
    };
    // *INDENT-ON*

    for (int bigPlan = 0; bigPlan < totalPlanCt; bigPlan++) {
        auto currPlans = getCurrPlans(bigPlan);
        if (checkPlans(currPlans)) {
            for (size_t i = 0; i < bargainers.size(); i++) {
                HrhBargainer *bargainer = bargainers[i];
                int plan = currPlans[i];
                bargainer->onHrhBargained(plan, mMount);
                mBargainResults[whos[i].index()].set(bargainer, plan);
            }

            return;
        }
    }

    util::p("princess: bargain failure");
}

void Princess::beg(std::array<TileCount, 4> &inits)
{
    for (Who begger : whos::ALL4) {
        TileCount stock(mMount.getStockA());
        for (Who rival : whos::ALL4) {
            if (begger == rival)
                continue;

            int r = rival.index();
            if (!mBargainResults[r].active())
                continue;

            int plan = mBargainResults[r].plan();
            HrhBargainer *bar = mBargainResults[r].bargainer();
            for (T34 t : tiles34::ALL34) {
                using Claim = HrhBargainer::Claim;
                Claim claim = bar->hrhBargainClaim(plan, t);
                if (claim == Claim::ALL || claim == Claim::FOUR)
                    stock.clear(t);
            }
        }

        std::optional<HrhInitFix> fix = mGirls[begger.index()]->onHrhBeg(mRand, mTable, stock);
        if (fix.has_value()) // ppriority ignored in 'beg' stage
            fixInit(inits[begger.index()], *fix);
    }
}

util::Stactor<int, 34> Princess::permutation(int size)
{
    util::Stactor<int, 34> res;
    for (int i = 0; i < size; i++)
        res.pushBack(i);

    std::shuffle(res.begin(), res.end(), mRand.getUrbg());
    return res;
}

std::array<Hand, 4> Princess::monkey(std::array<TileCount, 4> &inits)
{
    std::array<Hand, 4> res;
    std::array<Exist, 4> exists;

    for (auto &g : mGirls)
        g->onMonkey(exists, mTable);

    // checkInit() should never rely on a moutain remaining status
    // because hands are checked sequentially but not globally
    for (int w = 0; w < 4; w++) {
        for (int iter = 0; true; iter++) {
            Mount mount(mMount);
            TileCount init(inits[w]); // copy
            Exist exist(exists[w]); // copy

            mount.initFill(mRand, init, exist);
            Hand hand(init);

            // *INDENT-OFF*
            auto pass = [&](Who checker) {
                return mGirls[checker.index()]->checkInit(Who(w), hand, mTable, iter);
            };
            // *INDENT-ON*

            if (util::all(whos::ALL4, pass)) {
                mMount = mount;
                res[w] = hand;
                break;
            }
        }
    }

    return res;
}

void Princess::fixInit(TileCount &init, const HrhInitFix &fix)
{
    for (const T37 &t : fix.targets) {
        if (mMount.remainA(t) > 0) {
            init.inc(t, 1);
            mMount.initPopExact(t);
        }
    }

    for (const auto &load : fix.loads)
        mMount.loadB(load.tile, load.ct);
}

int Princess::BargainResult::plan() const
{
    return mPlan;
}

HrhBargainer *Princess::BargainResult::bargainer() const
{
    return mBargainer;
}

void Princess::BargainResult::set(HrhBargainer *b, int p)
{
    mBargainer = b;
    mPlan = p;
}

bool Princess::BargainResult::active() const
{
    return mBargainer != nullptr;
}



} // namespace saki
