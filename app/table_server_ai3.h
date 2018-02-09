#ifndef SAKI_APP_SERVER_AI3_H
#define SAKI_APP_SERVER_AI3_H

#include "table_server.h"
#include "../table/table_tester.h"

#include <iterator>



namespace saki
{



class TableServerAi3
{
public:
    using Ai3 = std::array<std::unique_ptr<TableDecider>, 3>;
    TableServerAi3(Table::InitConfig config, std::vector<TableObserver *> obs,
                   const TableEnv &env, Ai3 &&ais);

    using Msgs = std::vector<TableMsgContent>;
    using MsgBackInserter = std::back_insert_iterator<Msgs>;

    Msgs start();
    Msgs action(const Action &action);

private:
    Msgs filterMsgs(const TableServer::Msgs &srcs);
    void filterSystemMsg(const TableMsgContent &content, MsgBackInserter it);
    void filterUserMsg(const TableMsg &msg, MsgBackInserter it);
    void addBotAction(Who who, const Action &action, int nonce, MsgBackInserter it);
    void addFakeBotDelay(const TableMsgContent &content, MsgBackInserter it);

private:
    TableServer mServer;
    std::array<std::unique_ptr<TableDecider>, 3> mAis;
};



} // namespace saki



#endif // SAKI_APP_SERVER_AI3_H
