#ifndef SAKI_TABLE_H
#define SAKI_TABLE_H

#include "choices.h"
#include "kan_ctx.h"
#include "mount.h"
#include "table_env.h"
#include "table_view.h"
#include "table_observer.h"
#include "girl.h"
#include "../form/tile_count.h"
#include "../util/rand.h"

#include <memory>
#include <iostream>



namespace saki
{



struct Furiten
{
    bool sutehai = false;
    bool doujun = false;
    bool riichi = false;
    bool any() const { return sutehai || doujun || riichi; }
    bool none() const { return !any(); }
};



///
/// \brief Default-copy-constructable part of Table
///
class TablePrivate
{
protected:
    TablePrivate(std::array<int, 4> points,
                 Rule rule, Who tempDealer, const TableEnv &env);
    explicit TablePrivate(const TablePrivate &copy) = default;
    TablePrivate &operator=(const TablePrivate &assign) = delete;
    ~TablePrivate() = default;

protected:
    util::Rand mRand;
    Mount mMount;
    const Rule mRule;
    KanCtx mKanContext;
    TableFocus mFocus;

    std::array<int, 4> mPoints;
    std::array<int, 4> mRiichiHans;
    std::array<int, 4> mLayPositions;
    std::array<Furiten, 4> mFuritens;
    std::array<Hand, 4> mHands;
    std::array<River, 4> mRivers;
    std::array<std::bitset<24>, 4> mPickeds;
    std::array<Choices, 4> mChoicess;
    std::array<Action, 4> mActionInbox;
    std::array<int, 4> mNonces;

    int mRound = 0;
    int mExtraRound = -1; // work with beforeEast1()
    int mDeposit = 0;
    int mDice;

    Who mDealer;
    Who mInitDealer;

    bool mAllLast = false;
    bool mToChangeDealer = false;
    bool mPrevIsRyuu = false;
    bool mToEstablishRiichi;
    bool mToFlip;

    std::bitset<4> mIppatsuFlags;
    std::array<std::bitset<34>, 4> mGenbutsuFlags;

    const TableEnv &mEnv;
};



class Table : private TablePrivate
{
public:
    enum class CheckResult { OK, EXPIRED, ILLEGAL };

    struct InitConfig
    {
        std::array<int, 4> points;
        std::array<int, 4> girlIds;
        Rule rule;
        Who tempDealer;
    };

    explicit Table(InitConfig config, std::vector<TableObserver *> obs, const TableEnv &env);
    explicit Table(const Table &orig, std::vector<TableObserver *> obs);

    Table(const Table &copy) = delete;
    Table &operator=(const Table &assign) = delete;

    void start();
    void action(Who who, const Action &act, int nonce);
    CheckResult check(Who who, const Action &action, int nonce) const;

    const Hand &getHand(Who who) const;
    const River &getRiver(Who who) const;
    const Girl &getGirl(Who who) const;
    std::unique_ptr<TableView> getView(Who who) const;
    const Furiten &getFuriten(Who who) const;
    const std::array<int, 4> &getPoints() const;
    int getRound() const;
    int getExtraRound() const;
    TileCount visibleRemain(Who who) const;
    int riverRemain(T34 t) const;
    int getRank(Who who) const;
    const TableFocus &getFocus() const;
    const T37 &getFocusTile() const;
    bool genbutsu(Who whose, T34 t) const;
    bool lastDiscardLay() const;
    bool riichiEstablished(Who who) const;
    bool duringKan() const;
    bool isAllLast() const;
    bool beforeEast1() const;
    bool inIppatsuCycle() const;
    bool anyActivated() const;

    Who findGirl(Girl::Id id) const;
    Who getDealer() const;
    int getDice() const;
    int getDeposit() const;
    int getSelfWind(Who who) const;
    int getRoundWind() const;
    int getNonce(Who who) const;
    const Rule &getRule() const;
    FormCtx getFormCtx(Who who) const;
    const Choices &getChoices(Who who) const;
    const Mount &getMount() const;
    const TableEnv &getEnv() const;

    void popUp(Who who) const;

private:
    bool checkLegality(Who who, const Action &action) const;
    void process();
    void singleAction(Who who, const Action &act);

    void setupObservers(const std::vector<TableObserver *> obs);
    void nextRound();
    void clean();
    void rollDice();
    void deal();
    void flipKandoraIndic();
    void notifyFlipped();
    void tryDraw(Who who);
    void swapOut(Who who, const T37 &out);
    void spinOut(Who who);
    void barkOut(Who who, const T37 &out);
    void discardEffects(Who who, bool spin);
    void declareRiichi(Who who, const Action &action);
    bool finishRiichi();
    void chii(Who who, ActCode dir, const T37 &out, bool showAka5);
    void pon(Who who, const T37 &out, int showAka5);
    void daiminkan(Who who);
    void pick();
    void ankan(Who who, T34 tile);
    void kakan(Who who, int barkId);
    void finishKan(Who who);
    void activate();
    void everyonePassed();
    bool filterChoices();
    bool kanOverflow(Who kanner);
    bool noBarkYet() const;
    bool checkDeathWinds() const;
    void checkChankan(bool only13 = false);
    void checkBarkRon();
    void checkSutehaiFuriten();
    void passRon(Who who);
    bool checkNagashimangan(Who who) const;
    void exhaustRound(RoundResult result, const util::Stactor<Who, 4> &openers);
    void finishRound(const util::Stactor<Who, 4> &openers, Who gunner);
    void checkNextRound();
    void endTable();

private:
    std::array<std::unique_ptr<Girl>, 4> mGirls;
    std::vector<TableObserver *> mObservers;
};



} // namespace saki



#endif // SAKI_TABLE_H
