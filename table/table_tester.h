#ifndef SAKI_TABLE_TESTER_H
#define SAKI_TABLE_TESTER_H

#include "table.h"



namespace saki
{



class TableDecider
{
public:
    /// executes action before regarding 'abortTable'
    /// if want to abort with no action, use ActCode::NOTHING
    struct Decision
    {
        Action action;
        bool abortTable = false;
    };

    virtual ~TableDecider() = default;

    virtual Decision decide(const TableView &view) = 0;
};



class TableTester
{
public:
    explicit TableTester(Table &table, std::array<TableDecider *, 4> deciders);

    void run();

private:
    Table &mTable;
    std::array<TableDecider *, 4> mDeciders;
};



} // namespace saki



#endif // SAKI_TABLE_TESTER_H
