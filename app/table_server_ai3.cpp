#include "table_server_ai3.h"
#include "../ai/ai.h"



namespace saki
{



TableServerAi3::TableServerAi3(Table::InitConfig config, std::vector<TableObserver *> obs,
                               const TableEnv &env, Ai3 &&ais)
    : mServer(std::move(config), obs, env)
    , mAis(std::move(ais))
{
}

TableServerAi3::Msgs TableServerAi3::start()
{
    auto msgs = mServer.start();
    return filterMsgs(msgs);
}

TableServerAi3::Msgs TableServerAi3::action(const Action &action)
{
    int nonce = mServer.table().getNonce(whos::HUMAN);
    auto msgs = mServer.action(whos::HUMAN, action, nonce);
    return filterMsgs(msgs);
}

const Table &TableServerAi3::table()
{
    return mServer.table();
}

TableServerAi3::Msgs TableServerAi3::filterMsgs(const TableServer::Msgs &srcs)
{
    Msgs msgs;

    for (const TableMsg &msg : srcs) {
        if (msg.to.nobody()) {
            filterSystemMsg(msg.content, std::back_inserter(msgs));
            continue;
        }

        filterUserMsg(msg, std::back_inserter(msgs));
    }

    return msgs;
}

void TableServerAi3::filterSystemMsg(const TableMsgContent &content, MsgBackInserter it)
{
    std::string event = content.event();
    const auto &args = content.args();

    if (event == "riichi-auto") {
        Who riichier(args["Who"].get<int>());

        if (riichier.human())
            *it++ = TableMsgContent::JustPause { 300 };

        int nonce = mServer.table().getNonce(riichier);
        addBotAction(riichier, Action(ActCode::SPIN_OUT), nonce, it);
    } else if (event == "round-start-log") {
        int round = args["round"];
        int extra = args["extra"];
        int dealer = args["dealer"];
        int deposit = args["deposit"];
        bool allLast = args["allLast"];
        uint32_t seed = args["seed"];
        std::cout << round << '.' << extra << (allLast ? "al" : "")
                  << " d=" << dealer << " depo=" << deposit
                  << " seed=" << seed << std::endl;
    } else {
        // ignoring other types of system msgs
    }
}

void TableServerAi3::filterUserMsg(const TableMsg &msg, TableServerAi3::MsgBackInserter it)
{
    std::string event = msg.content.event();
    if (event == "activated") {
        auto view = mServer.table().getView(msg.to);

        // must double-check because msg might be expired
        if (view->myChoices().any()) {
            if (msg.to.human()) {
                *it++ = msg.content;
            } else {
                auto d = mAis[msg.to.index() - 1]->decide(*view);
                int nonce = msg.content.args()["nonce"];
                addBotAction(msg.to, d.action, nonce, it);
            }
        }
    } else { // non-activation observing
        if (msg.to.human()) {
            addFakeBotDelay(msg.content, it);
            *it++ = msg.content;
        }
    }
}

void TableServerAi3::addBotAction(Who who, const Action &action, int nonce, MsgBackInserter it)
{
    auto subMsgs = filterMsgs(mServer.action(who, action, nonce));
    for (const auto &msg : subMsgs)
        *it++ = msg;
}

void TableServerAi3::addFakeBotDelay(const TableMsgContent &content, MsgBackInserter it)
{
    std::string event = content.event();
    const nlohmann::json &args = content.args();

    if (event == "diced") {
        // strictly speaking, getting from table breaks the msg queue order
        // but usually does no much harm
        if (!mServer.table().getDealer().human())
            *it++ = TableMsgContent::JustPause { 700 };
    } else if (event == "discarded") {
        Who who(args["who"].get<int>());
        if (!who.human())
            *it++ = TableMsgContent::JustPause { 300 };
    } else if (event == "riichi-called") {
        Who who(args["who"].get<int>());
        if (!who.human())
            *it++ = TableMsgContent::JustPause { 300 };
    } else if (event == "barked") {
        Who who(args["who"].get<int>());
        if (!who.human())
            *it++ = TableMsgContent::JustPause { 500 };
    } else if (event == "round-ended") {
        std::string result = args[field::RESULT];
        if (result == "TSUMO" || result == "RON" || result == "SCHR") {
            nlohmann::json openers = args[field::OPENERS];
            Who onlyOpener(openers[0].get<int>());
            if (openers.size() > 1 || !onlyOpener.human())
                *it++ = TableMsgContent::JustPause { 700 };
        }
    }
}



} // namespace saki
