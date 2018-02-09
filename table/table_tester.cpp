#include "table_tester.h"



namespace saki
{



TableTester::TableTester(Table &table, std::array<TableDecider *, 4> deciders)
    : mTable(table)
    , mDeciders(deciders)
{
}

void TableTester::run(bool fromHalfWay)
{
    if (!fromHalfWay)
        mTable.start();

    // *INDENT-OFF*
    auto pickBusy = [&]() {
        for (Who who : whos::ALL4)
            if (mTable.getView(who)->myChoices().any())
                return who;

        return Who();
    };
    // *INDENT-ON*

    for (Who who = pickBusy(); who.somebody(); who = pickBusy()) {
        auto view = mTable.getView(who);
        auto decision = mDeciders[who.index()]->decide(*view);

        if (decision.action.act() != ActCode::NOTHING)
            mTable.action(who, decision.action, mTable.getNonce(who));

        if (decision.abortTable)
            return;
    }
}



} // namespace saki
