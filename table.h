#ifndef SAKI_TABLE_H
#define SAKI_TABLE_H

#include "who.h"
#include "choices.h"
#include "mount.h"
#include "girl.h"
#include "table_view.h"
#include "table_operator.h"
#include "table_observer.h"
#include "tile_count.h"
#include "rand.h"

#include <memory>
#include <iostream>



namespace saki
{



class KanContext
{
public:
    KanContext() = default;
    explicit KanContext(const KanContext &copy) = default;
    KanContext &operator=(const KanContext &copy) = default;
    ~KanContext() = default;

    bool during() const;
    bool duringMinkan() const;
    bool duringAnkan() const;
    Who pao() const;

    void enterDaiminkan(Who pao);
    void enterAnkan();
    void enterKakan();

    /// \brief Called when a discard or another kan happens
    void leave();

private:
    bool mDuring = false;
    bool mMinkan;
    Who mPao;
};



struct Furiten
{
    bool sutehai = false;
    bool doujun = false;
    bool riichi = false;
    bool any() const { return sutehai || doujun || riichi; }
    bool none() const { return !any(); }
};



class TablePrivate
{
protected:
    TablePrivate(const std::array<int, 4> &points,
                 RuleInfo rule, Who tempDealer);
    explicit TablePrivate(const TablePrivate &copy) = default;
    TablePrivate &operator=(const TablePrivate &assign) = delete;
    ~TablePrivate() = default;

protected:
    Rand mRand;
    Mount mMount;
    const RuleInfo mRule;
    KanContext mKanContext;
    TableFocus mFocus;

    std::array<int, 4> mPoints;
    std::array<int, 4> mRiichiHans;
    std::array<int, 4> mLayPositions;
    std::array<Furiten, 4> mFuritens;
    std::array<Hand, 4> mHands;
    std::array<util::Stactor<T37, 24>, 4> mRivers;
    std::array<std::bitset<24>, 4> mPickeds;
    std::array<Choices, 4> mChoicess;
    std::array<Action, 4> mActionInbox;

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
};



class Table : private TablePrivate
{
public:
    explicit Table(const std::array<int, 4> &points,
                   const std::array<int, 4> &girlIds,
                   const std::array<TableOperator*, 4> &operators,
                   const std::vector<TableObserver*> &observers,
                   RuleInfo rule, Who tempDealer);

    explicit Table(const Table &orig,
                   const std::array<TableOperator*, 4> &operators,
                   const std::vector<TableObserver*> &observers,
                   Who toki, const Choices &clean);

    Table(const Table &copy) = delete;
    Table &operator=(const Table &assign) = delete;

    void start();
    void action(Who who, const Action &act);
    bool check(Who who, const Action &action) const;

    const Hand &getHand(Who who) const;
    const util::Stactor<T37, 24> &getRiver(Who who) const;
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

    Who findGirl(Girl::Id id) const;
    Who getDealer() const;
    int getDice() const;
    int getDeposit() const;
    int getSelfWind(Who who) const;
    int getRoundWind() const;
    const RuleInfo &getRuleInfo() const;
    PointInfo getPointInfo(Who who) const;
    const Choices &getChoices(Who who) const;
    const Mount &getMount() const;

    void popUp(Who who) const;

private:
    void process();
    void singleAction(Who who, const Action &act);

    void nextRound();
    void clean();
    void rollDice();
    void deal();
    void flip();
    void tryDraw(Who who);
    void swapOut(Who who, const T37 &out);
    void spinOut(Who who);
    void barkOut(Who who, const T37 &out);
    void onDiscarded();
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
    bool anyActivated() const;
    bool kanOverflow(Who kanner);
    bool noBarkYet() const;
    bool checkDeathWinds() const;
    void checkChankan(bool only13 = false);
    void checkBarkRon();
    void checkSutehaiFuriten();
    void passRon(Who who);
    bool nagashimangan(Who who) const;
    void exhaustRound(RoundResult result, const std::vector<Who> &openers);
    void finishRound(const std::vector<Who> &openers, Who gunner);
    void endOrNext();
    void endTable();

private:
    std::array<std::unique_ptr<Girl>, 4> mGirls;
    std::array<TableOperator*, 4> mOperators;
    std::vector<TableObserver*> mObservers;
};



} // namespace saki



#endif // SAKI_TABLE_H


