#include "table_server.h"
#include "../util/string_enum.h"



namespace saki
{



using namespace nlohmann;
using namespace field;

template<typename T>
void rotate(T &arr)
{
    auto temp = arr[0];
    arr[0] = arr[1];
    arr[1] = arr[2];
    arr[2] = arr[3];
    arr[3] = temp;
}

TableServer::TableServer(Table::InitConfig config,
                         std::vector<TableObserver *> obs, const TableEnv &env)
    : mTable(std::move(config), (obs.push_back(this), obs), env)
    , mNonces{0, 0, 0, 0}
{

}

TableServer::Msgs TableServer::start()
{
    mTable.start();
    pushActivationMsgs();
    return popMsgs();
}

TableServer::Msgs TableServer::action(Who who, const Action &action, int nonce)
{
    switch (mTable.check(who, action, nonce)) {
    case Table::CheckResult::OK:
        mTable.action(who, action, nonce);
        pushActivationMsgs();
        break;
    case Table::CheckResult::EXPIRED:
        pushActionExpired(who);
        break;
    case Table::CheckResult::ILLEGAL:
        pushActionIllegal(who);
        break;
    }

    return popMsgs();
}

TableServer::Msgs TableServer::resume(Who comer)
{
    if (mTable.beforeEast1())
        return popMsgs(); // no serious info needs to be provided

    json args;

    args["whoDrawn"] = -1;
    args["barkss"] = json::array();
    args["rivers"] = json::array();
    args["riichiBars"] = json::array();
    args["dice"] = mTable.getDice();

    if (mTable.getDice() > 0) {
        for (Who who : whos::ALL4) {
            const Hand &hand = mTable.getHand(who);
            int pers = who.turnFrom(comer);
            if (hand.hasDrawn()) {
                args["whoDrawn"] = pers;
                if (who == comer)
                    args["drawn"] = stringOf(hand.drawn());
            }

            if (who == comer)
                args["myHand"] = hand.closed().t37s13(true).range();

            args["barkss"][pers] = hand.barks();
            args["rivers"][pers] = mTable.getRiver(who).range();
            args["riichiBars"][pers] = mTable.riichiEstablished(who);
        }

        args["drids"] = mTable.getMount().getDrids().range();
    }

    const auto &pts = mTable.getPoints();
    args["points"] = json {
        pts[comer.index()],
        pts[comer.right().index()],
        pts[comer.cross().index()],
        pts[comer.left().index()],
    };

    args["girlKeys"] = json::array();
    for (Who who: { comer, comer.right(), comer.cross(), comer.left() }) {
        json key;
        key["id"] = static_cast<int>(mTable.getGirl(who).getId());
        key["path"] = "";
        args["girlKeys"].push_back(key);
    }

    args["wallRemain"] = mTable.getMount().remainPii();
    args["deadRemain"] = mTable.getMount().remainRinshan();

    args["round"] = mTable.getRound();
    args["extraRound"] = mTable.getExtraRound();
    args["dealer"] = mTable.getDealer().turnFrom(comer);
    args["allLast"] = mTable.isAllLast();
    args["deposit"] = mTable.getDeposit();

    pushPeerMsg(comer, "resume", args);

    if (mTable.getView(comer)->myChoices().any())
        pushActivationMsg(comer, true);

    return popMsgs();
}

void TableServer::onTableEvent(const Table &table, const TE::TableStarted &event)
{
    (void) event;

    pushPointsChanged(table);
}

void TableServer::onTableEvent(const Table &table, const TE::FirstDealerChosen &event)
{
    (void) table;

    json args;
    for (Who to : whos::ALL4) {
        args["dealer"] = event.who.turnFrom(to);
        pushPeerMsg(to, util::stringOf(event.TYPE), args);
    }
}

void TableServer::onTableEvent(const Table &table, const TE::RoundStarted &event)
{
    (void) table;

    json args;
    args["round"] = event.round;
    args["extra"] = event.extraRound;
    args["allLast"] = event.allLast;
    args["deposit"] = event.deposit;
    for (Who to : whos::ALL4) {
        args["dealer"] = event.dealer.turnFrom(to);
        pushPeerMsg(to, util::stringOf(event.TYPE), args);
    }

    args["dealer"] = event.dealer.index();
    args["seed"] = event.seed;
    pushSystemMsg("round-start-log", args);
}

void TableServer::onTableEvent(const Table &table, const TE::Cleaned &event)
{
    (void) table;
    (void) event;

    pushBroadcastMsg(util::stringOf(event.TYPE), json::object());
}

void TableServer::onTableEvent(const Table &table, const TE::Diced &event)
{
    (void) table;

    json args;
    args["die1"] = event.die1;
    args["die2"] = event.die2;
    pushBroadcastMsg(util::stringOf(event.TYPE), args);
}

void TableServer::onTableEvent(const Table &table, const TE::Dealt &event)
{
    (void) event;

    for (Who to : whos::ALL4) {
        const auto &init = table.getHand(to).closed().t37s13(true);
        json args;
        args["init"] = init.range();
        pushPeerMsg(to, util::stringOf(event.TYPE), args);
    }
}

void TableServer::onTableEvent(const Table &table, const TE::Flipped &event)
{
    (void) event;

    json args;
    args["newIndic"] = stringOf(table.getMount().getDrids().back());
    pushBroadcastMsg(util::stringOf(event.TYPE), args);
}

void TableServer::onTableEvent(const Table &table, const TE::Drawn &event)
{
    const T37 &in = table.getHand(event.who).drawn();

    for (Who to : whos::ALL4) {
        json args;
        args["who"] = event.who.turnFrom(to);

        if (table.duringKan())
            args["rinshan"] = true;

        if (to == event.who)
            args["tile"] = stringOf(in);

        pushPeerMsg(to, util::stringOf(event.TYPE), args);
    }
}

void TableServer::onTableEvent(const Table &table, const TE::Discarded &event)
{
    Who discarder = table.getFocus().who();
    const T37 &out = table.getFocusTile();
    bool lay = table.lastDiscardLay();

    json args;
    args["tile"] = stringOf(out, lay);
    args["spin"] = event.spin;
    for (Who to : whos::ALL4) {
        args["who"] = discarder.turnFrom(to);
        pushPeerMsg(to, util::stringOf(event.TYPE), args);
    }
}

void TableServer::onTableEvent(const Table &table, const TE::RiichiCalled &event)
{
    (void) table;

    for (Who to : whos::ALL4) {
        json args;
        args["who"] = event.who.turnFrom(to);
        pushPeerMsg(to, util::stringOf(event.TYPE), args);
    }
}

void TableServer::onTableEvent(const Table &table, const TE::RiichiEstablished &event)
{
    (void) table;

    for (Who to : whos::ALL4) {
        json args;
        args["who"] = event.who.turnFrom(to);
        pushPeerMsg(to, util::stringOf(event.TYPE), args);
    }
}

void TableServer::onTableEvent(const Table &table, const TE::Barked &event)
{
    Who from = event.bark.isCpdmk() ? table.getFocus().who() : Who();

    json args;
    args["actStr"] = util::stringOf(event.bark.type());
    args["bark"] = event.bark;
    args["spin"] = event.spin;
    for (Who to : whos::ALL4) {
        args["who"] = event.who.turnFrom(to);
        args["fromWhom"] = from.somebody() ? from.turnFrom(to) : -1;
        pushPeerMsg(to, util::stringOf(event.TYPE), args);
    }
}

void TableServer::onTableEvent(const Table &table, const TE::RoundEnded &event)
{
    using RR = RoundResult;

    // form and hand lists have same order as openers
    // but they don't need to be rotated since openers
    // are not rotated but changed by value
    json formsList = json::array();
    json handsList = json::array();

    for (Who who : event.openers) {
        const Hand &hand = table.getHand(who);

        json handMap;
        handMap["closed"] = hand.closed().t37s13(true).range();
        handMap["barks"] = hand.barks();

        if (event.result == RR::TSUMO || event.result == RR::KSKP)
            handMap[PICK] = stringOf(hand.drawn());
        else if (event.result == RR::RON || event.result == RR::SCHR)
            handMap[PICK] = stringOf(table.getFocusTile());

        handsList.emplace_back(handMap);
    }

    for (const Form &form : event.forms) {
        json formMap;
        formMap["spell"] = form.spell();
        formMap["charge"] = form.charge();
        formsList.emplace_back(formMap);
    }

    json args;
    args[RESULT] = util::stringOf(event.result);
    args[HANDS] = handsList;
    args[FORMS] = formsList;
    args["urids"] = table.getMount().getUrids().range();
    for (Who to : whos::ALL4) {
        args[OPENERS] = json::array();
        for (Who who : event.openers)
            args[OPENERS].push_back(who.turnFrom(to));

        Who gunner = event.gunner;
        args["gunner"] = gunner.somebody() ? gunner.turnFrom(to) : -1;
        pushPeerMsg(to, util::stringOf(event.TYPE), args);
    }
}

void TableServer::onTableEvent(const Table &table, const TE::PointsChanged &event)
{
    (void) event;

    pushPointsChanged(table);
}

void TableServer::onTableEvent(const Table &table, const TE::TableEnded &event)
{
    (void) table;

//	tableEndStat(scores);

    json args;
    args["scores"] = event.scores;
    for (Who to : whos::ALL4) {
        json rankList;
        for (Who who : event.ranks)
            rankList.push_back(who.turnFrom(to));

        args["rank"] = rankList;
        pushPeerMsg(to, util::stringOf(event.TYPE), args);
        rotate(args["scores"]);
    }

    pushSystemMsg("game-over", json());
}

void TableServer::onTableEvent(const Table &table, const TE::PoppedUp &event)
{
    json args;
    args["str"] = table.getGirl(event.who).popUpStr();
    pushPeerMsg(event.who, util::stringOf(event.TYPE), args);
}

const Table &TableServer::table() const
{
    return mTable;
}

void TableServer::pushActivationMsgs()
{
    for (Who who : whos::ALL4)
        if (mTable.getView(who)->myChoices().any())
            pushActivationMsg(who);
}

void TableServer::pushActivationMsg(Who who, bool isResume)
{
    using AC = ActCode;
    using Mode = Choices::Mode;

    const auto view = mTable.getView(who);
    const Choices &choices = view->myChoices();

    if (!isResume && mTable.getNonce(who) == mNonces[who.index()])
        return; // still waiting for input

    mNonces[who.index()] = mTable.getNonce(who);

    if (view->riichiEstablished(who) && choices.spinOnly()) {
        json args;
        args["Who"] = who.index();
        args["Nonce"] = mTable.getNonce(who);
        pushSystemMsg("riichi-auto", args);
        return;
    }

    json map;
    int focusWho = -1;

    switch (choices.mode()) {
    case Mode::WATCH:
        break;
    case Mode::IRS_CHECK:
        activateIrsCheck(map, *view);
        break;
    case Mode::DICE:
        map[util::stringOf(AC::DICE)] = true;
        break;
    case Mode::DRAWN:
        activateDrawn(map, *view);
        break;
    case Mode::BARK:
        focusWho = view->getFocus().who().turnFrom(who);
        activateBark(map, *view);
        break;
    case Mode::END:
        if (choices.can(AC::END_TABLE))
            map[util::stringOf(AC::END_TABLE)] = true;

        if (choices.can(AC::NEXT_ROUND))
            map[util::stringOf(AC::NEXT_ROUND)] = true;

        break;
    }

    if (choices.can(AC::IRS_CLICK))
        map[util::stringOf(AC::IRS_CLICK)] = true;

    json args;
    args["action"] = map;
    args["lastDiscarder"] = focusWho;
    args["nonce"] = mTable.getNonce(who);
    // args["green"] = view->myChoices().forwardAll();
    pushPeerMsg(who, "activated", args);
}

void TableServer::pushPointsChanged(const Table &table)
{
    json args;
    args["points"] = table.getPoints();
    for (Who to : whos::ALL4) {
        pushPeerMsg(to, util::stringOf(TE::Type::POINTS_CHANGED), args);
        rotate(args["points"]);
    }
}

void TableServer::pushActionExpired(Who who)
{
    json args;
    args["Who"] = who.index();
    pushSystemMsg("action-expired", args);
}

void TableServer::pushActionIllegal(Who who)
{
    json args;
    args["Who"] = who.index();
    pushSystemMsg("action-illegal", args);
}

void TableServer::pushPeerMsg(Who who, const char *event, const json &args)
{
    TableMsgContent msg(event, args);
    mMsgs.push_back(TableMsg { who, msg });
}

void TableServer::pushBroadcastMsg(const char *event, const json &args)
{
    TableMsgContent msg(event, args);
    for (Who who : whos::ALL4)
        mMsgs.push_back(TableMsg { who, msg });
}

void TableServer::pushSystemMsg(const char *type, const json &args)
{
    TableMsgContent msg(type, args);
    mMsgs.push_back(TableMsg { Who(), msg });
}

TableServer::Msgs TableServer::popMsgs()
{
    auto msgs = mMsgs;
    mMsgs.clear();
    return msgs;
}

void TableServer::activateDrawn(json &map, const TableView &view)
{
    using AC = ActCode;

    for (AC ac : { AC::SPIN_OUT, AC::SPIN_RIICHI, AC::TSUMO, AC::RYUUKYOKU })
        if (view.myChoices().can(ac))
            map[util::stringOf(ac)] = true;

    const Choices::ModeDrawn &mode = view.myChoices().drawn();

    if (mode.swapOut)
        map[util::stringOf(AC::SWAP_OUT)] = (1 << 13) - 1;

    if (!mode.swapRiichis.empty()) {
        const auto &closed = view.myHand().closed();
        map[util::stringOf(AC::SWAP_RIICHI)] = createSwapMask(closed, mode.swapRiichis);
    }

    if (!mode.ankans.empty())
        map[util::stringOf(AC::ANKAN)] = mode.ankans.range();

    if (!mode.kakans.empty()) {
        std::vector<int> kakans;
        for (int i : mode.kakans)
            kakans.push_back(i);

        map[util::stringOf(AC::KAKAN)] = kakans;
    }
}

void TableServer::activateBark(json &map, const TableView &view)
{
    using AC = saki::ActCode;

    std::array<AC, 7> just {
        AC::PASS,
        AC::CHII_AS_LEFT, AC::CHII_AS_MIDDLE, AC::CHII_AS_RIGHT,
        AC::PON, AC::DAIMINKAN, AC::RON
    };

    for (AC ac : just)
        if (view.myChoices().can(ac))
            map[util::stringOf(ac)] = true;
}

void TableServer::activateIrsCheck(json &map, const TableView &view)
{
    const Choices::ModeIrsCheck &mode = view.myChoices().irsCheck();

    json args;
    args["name"] = mode.name;

    json items;
    for (const auto &item : mode.list)
        items.push_back(item);

    args["items"] = items;

    map[util::stringOf(saki::ActCode::IRS_CHECK)] = args;
}



} // namespace saki
