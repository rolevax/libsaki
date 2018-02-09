#ifndef SAKI_APP_SERVER_H
#define SAKI_APP_SERVER_H

#include "table_msg.h"
#include "../table/table.h"



namespace saki
{



class TableServer : public TableObserverDispatched
{
public:
    using Msgs = std::vector<TableMsg>;

    explicit TableServer(Table::InitConfig config, std::vector<TableObserver *> obs,
                         const TableEnv &env);

    Msgs start();
    Msgs action(Who who, const Action &action, int nonce);
    Msgs resume(Who who);

    void onTableEvent(const Table &table, const TE::TableStarted &event) final;
    void onTableEvent(const Table &table, const TE::FirstDealerChosen &event) final;
    void onTableEvent(const Table &table, const TE::RoundStarted &event) final;
    void onTableEvent(const Table &table, const TE::Cleaned &event) final;
    void onTableEvent(const Table &table, const TE::Diced &event) final;
    void onTableEvent(const Table &table, const TE::Dealt &event) final;
    void onTableEvent(const Table &table, const TE::Flipped &event) final;
    void onTableEvent(const Table &table, const TE::Drawn &event) final;
    void onTableEvent(const Table &table, const TE::Discarded &event) final;
    void onTableEvent(const Table &table, const TE::RiichiCalled &event) final;
    void onTableEvent(const Table &table, const TE::RiichiEstablished &event) final;
    void onTableEvent(const Table &table, const TE::Barked &event) final;
    void onTableEvent(const Table &table, const TE::RoundEnded &event) final;
    void onTableEvent(const Table &table, const TE::PointsChanged &event) final;
    void onTableEvent(const Table &table, const TE::TableEnded &event) final;
    void onTableEvent(const Table &table, const TE::PoppedUp &event) final;

    const Table &table() const;

private:
    void pushActivationMsgs();
    void pushActivationMsg(Who who);
    void pushPointsChanged(const Table &table);
    void pushActionExpired(Who who);
    void pushActionIllegal(Who who);

    void pushPeerMsg(Who who, const char *event, const nlohmann::json &args);
    void pushBroadcastMsg(const char *event, const nlohmann::json &args);
    void pushSystemMsg(const char *type, const nlohmann::json &args);
    Msgs popMsgs();

    void activateDrawn(nlohmann::json &map, const TableView &view);
    void activateBark(nlohmann::json &map, const TableView &view);
    void activateIrsCheck(nlohmann::json &map, const TableView &view);

private:
    Table mTable;
    Msgs mMsgs;
    std::array<int, 4> mNonces;
};



} // namespace saki



#endif // SAKI_APP_TABLE_SERVER_H
