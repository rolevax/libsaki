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

    while (mTable.anyActivated()) {
        for (Who who : whos::ALL4) {
            auto view = mTable.getView(who);
            if (view->myChoices().any()) {
                auto decision = mDeciders[who.index()]->decide(*view);

                if (decision.action.act() != ActCode::NOTHING)
                    mTable.action(who, decision.action);

                if (decision.abortTable)
                    return;
            }
        }
    }
}



} // namespace saki
