#include "bmark.h"
#include "../ai/ai.h"
#include "../util/misc.h"



namespace saki
{



void Bmark::test(Girl::Id girlId)
{
    Bmark ob;

    std::array<int, 4> points { 25000, 25000, 25000, 25000 };
    std::array<int, 4> girlIds { static_cast<int>(girlId), 0, 0, 0 };
    std::array<std::unique_ptr<Ai>, 4> ais;
    std::array<TableOperator*, 4> ops;
    std::vector<TableObserver*> obs { &ob };
    Rule rule;

    for (int iter = 0; iter < 20; iter++) {
        for (int w = 0; w < 4; w++) {
            ais[w].reset(Ai::create(Who(w), Girl::Id(girlIds[w])));
            ops[w] = ais[w].get();
        }

        Table table(points, girlIds, ops, obs, rule, Who(0));
        table.start();
    }
}

void Bmark::onTableEnded(const std::array<Who, 4> &ranks, const std::array<int, 4> &scores)
{
    int index = std::find(ranks.begin(), ranks.end(), Who(0)) - ranks.begin();
    util::p("end rank", index + 1, "score", scores[index]);
}



} // namespace saki


