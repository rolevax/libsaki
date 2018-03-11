#include "table.h"
#include "princess.h"
#include "table_view_real.h"
#include "../util/misc.h"
#include "../util/debug_cheat.h"

#include <numeric>
#include <cstdlib>
#include <ctime>



namespace saki
{



TablePrivate::TablePrivate(std::array<int, 4> points,
                           Rule rule, Who tempDealer, const TableEnv &env)
    : mMount(rule.akadora)
    , mRule(rule)
    , mPoints(points)
    , mInitDealer(tempDealer)
    , mEnv(env)
{
    // feature: server nonce starts from 1
    //          such that client nonce can simply start from 0
    mNonces.fill(1);
}



///
/// \brief Create a new table
/// \param config Initial configuration
/// \param obs Observers
/// \param env Environment provider
///
Table::Table(InitConfig config, std::vector<TableObserver *> obs, const TableEnv &env)
    : TablePrivate(config.points, config.rule, config.tempDealer, env)
{
    assert(!util::has(mObservers, nullptr));

    for (int w = 0; w < 4; w++)
        mGirls[w] = Girl::create(Who(w), config.girlIds[w]);

    setupObservers(obs);

    // to choose real init dealer
    mChoicess[mInitDealer.index()].setDice();
}

///
/// \brief Clone a table and replace all its observers
///
Table::Table(const Table &orig, std::vector<TableObserver *> obs)
    : TablePrivate(orig)
{
    for (int w = 0; w < 4; w++)
        mGirls[w] = orig.mGirls[w]->clone();

    setupObservers(obs);
}

///
/// \brief Start the table
///
void Table::start()
{
    TableEvent event = TableEvent::TableStarted { mRand.state() };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    activate();
}

///
/// \brief Input an action to the table
///
void Table::action(Who who, const Action &act, int nonce)
{
    assert(check(who, act, nonce) == CheckResult::OK);

    int w = who.index();

    if (mGirls[w]->irsReady()) {
        bool handled = mGirls[w]->handleIrs(*this, mMount, act);
        if (!handled) {
            assert(!mGirls[w]->irsReady());
            action(who, act, nonce);
            return;
        }

        mNonces[w]++;
    } else {
        for (auto &g : mGirls)
            g->onInbox(who, act);

        if (act.act() == ActCode::PASS && mChoicess[w].can(ActCode::RON))
            passRon(who);

        mActionInbox[w] = act;
        mChoicess[w].setWatch();
        mNonces[w]++;
    }

    if (!anyActivated()) {
        process();
        activate();
    }
}

///
/// \brief Validate a user input
///
Table::CheckResult Table::check(Who who, const Action &action, int nonce) const
{
    if (nonce < getNonce(who))
        return CheckResult::EXPIRED;

    return checkLegality(who, action) ? CheckResult::OK : CheckResult::ILLEGAL;
}

const Hand &Table::getHand(Who who) const
{
    return mHands[who.index()];
}

const River &Table::getRiver(Who who) const
{
    return mRivers[who.index()];
}

std::unique_ptr<TableView> Table::getView(Who who) const
{
    // use polymorphic views in the future
    return util::unique<TableViewReal>(*this, who);
}

const Furiten &Table::getFuriten(Who who) const
{
    return mFuritens[who.index()];
}

const Girl &Table::getGirl(Who who) const
{
    return *mGirls[who.index()];
}

const std::array<int, 4> &Table::getPoints() const
{
    return mPoints;
}

int Table::getRound() const
{
    return mRound;
}

int Table::getExtraRound() const
{
    return mExtraRound;
}

TileCount Table::visibleRemain(Who who) const
{
    TileCount res(mRule.akadora);

    for (int w = 0; w < 4; w++) {
        for (const T37 &t : mRivers[w])
            res.inc(t, -1);

        for (const M37 &m : mHands[w].barks()) {
            const auto &ts = m.tiles();
            for (int i = 0; i < static_cast<int>(ts.size()); i++)
                if (i != m.layIndex()) // exclude picked tiles (counted in river)
                    res.inc(ts[i], -1);
        }
    }

    res -= mHands[who.index()].closed();

    if (mHands[who.index()].hasDrawn())
        res.inc(mHands[who.index()].drawn(), -1);

    for (const T37 &t : mMount.getDrids())
        res.inc(t, -1);

    return res;
}

int Table::riverRemain(T34 t) const
{
    int res = 4;
    for (int w = 0; w < 4; w++) {
        res -= std::count(mRivers[w].begin(), mRivers[w].end(), t);

        for (const M37 &m : mHands[w].barks()) {
            const auto &ts = m.tiles();
            for (int i = 0; i < static_cast<int>(ts.size()); i++)
                if (i != m.layIndex() && ts[i] == t)
                    res--;
        }
    }

    const auto &drids = mMount.getDrids();
    res -= std::count(drids.begin(), drids.end(), t);

    assert(0 <= res && res <= 4);
    return res;
}

///
/// \brief Get the ranking of a player
/// \param who The player
/// \return 1 ~ 4
///
int Table::getRank(Who who) const
{
    int rank = 1;
    int myPoint = mPoints[who.index()];
    for (int w = 0; w < 4; w++) {
        if (w == who.index())
            continue;

        int herPoint = mPoints[w];
        if (herPoint > myPoint) {
            rank++; // rank falls down
        } else if (herPoint == myPoint) {
            int myDist = who.turnFrom(mInitDealer);
            int herDist = Who(w).turnFrom(mInitDealer);
            if (herDist < myDist)
                rank++;
        }
    }

    return rank;
}

const TableFocus &Table::getFocus() const
{
    return mFocus;
}

const T37 &Table::getFocusTile() const
{
    int w = mFocus.who().index();
    return mFocus.isDiscard() ? mRivers[w].back()
                              : mHands[w].barks()[mFocus.barkId()][3];
}

///
/// \deprecated Will be replaced by TableMirror and AiMemo
///
bool Table::genbutsu(Who whose, T34 t) const
{
    return mGenbutsuFlags[whose.index()].test(t.id34());
}

///
/// \return true if the last discard is a riichi-declaring tile
///         or its replacing marker
///
bool Table::lastDiscardLay() const
{
    assert(mFocus.isDiscard());
    int w = mFocus.who().index();
    return static_cast<int>(mRivers[w].size()) - 1 == mLayPositions[w];
}

bool Table::riichiEstablished(Who who) const
{
    return mRiichiHans[who.index()] != 0;
}

///
/// \return true iff someone is performing a kan
///
bool Table::duringKan() const
{
    return mKanContext.during();
}

bool Table::isAllLast() const
{
    return mAllLast;
}

///
/// \return true iff the very first has not started yet
///
bool Table::beforeEast1() const
{
    return mExtraRound == -1;
}

bool Table::inIppatsuCycle() const
{
    return mIppatsuFlags.any();
}

///
/// \brief Checks if the table is waiting for any user input
/// \return true iff waiting
///
bool Table::anyActivated() const
{
    return util::any(whos::ALL4, [this](Who who) { return getChoices(who).any(); });
}

Who Table::findGirl(Girl::Id id) const
{
    for (int w = 0; w < 4; w++)
        if (mGirls[w]->getId() == id)
            return Who(w);

    return Who();
}

Who Table::getDealer() const
{
    return mDealer;
}

///
/// \brief Get the sum of the two dice numbers of this round
/// \return The dice sum
///
int Table::getDice() const
{
    return mDice;
}

int Table::getDeposit() const
{
    return mDeposit;
}

int Table::getSelfWind(Who who) const
{
    return who.turnFrom(mDealer) + 1;
}

int Table::getRoundWind() const
{
    return (mRound / 4) % 4 + 1;
}

int Table::getNonce(Who who) const
{
    return mNonces[who.index()];
}

const Rule &Table::getRule() const
{
    return mRule;
}

///
/// \brief Get the context for counting the points of a hand
/// \param who The player whose hand is to be calculated
/// \return The form context
///
FormCtx Table::getFormCtx(Who who) const
{
    FormCtx ctx;

    if (riichiEstablished(who)) {
        ctx.riichi = mRiichiHans[who.index()];
        ctx.ippatsu = mIppatsuFlags.test(who.index());
    }

    ctx.bless = noBarkYet() && mRivers[who.index()].empty();
    ctx.duringKan = mKanContext.during();
    ctx.emptyMount = mMount.remainPii() == 0;

    ctx.roundWind = getRoundWind();
    ctx.selfWind = getSelfWind(who);
    ctx.extraRound = mExtraRound;

    return ctx;
}

const Choices &Table::getChoices(Who who) const
{
    int w = who.index();
    return mGirls[w]->irsReady() ? mGirls[w]->irsChoices() : mChoicess[w];
}

const Mount &Table::getMount() const
{
    return mMount;
}

const TableEnv &Table::getEnv() const
{
    return mEnv;
}

///
/// \brief Pop up extra texts onto the screen
/// \param who Player to whom the text is visible
///
void Table::popUp(Who who) const
{
    TableEvent event = TableEvent::PoppedUp { who };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);
}

///
/// \brief Checks if an action is legal at current timepoint
/// \param who The player who performs this action
/// \param action The action to be checked
/// \return true iff legal
///
bool Table::checkLegality(Who who, const Action &action) const
{
    const Choices &choices = getChoices(who);

    if (!choices.can(action.act()))
        return false;

    using AC = ActCode;

    // check action arguments
    switch (choices.mode()) {
    case Choices::Mode::DRAWN:
        switch (action.act()) {
        case AC::SWAP_OUT:
            return util::has(mHands[who.index()].closed().t37s13(), action.t37());
        case AC::SWAP_RIICHI:
            return util::has(choices.drawn().swapRiichis, action.t37());
        case AC::ANKAN:
            return util::has(choices.drawn().ankans, action.t34());
        case AC::KAKAN:
            return util::has(choices.drawn().kakans, action.barkId());
        default:
            return true;
        }

    case Choices::Mode::BARK:
        if (action.isCp())
            return mHands[who.index()].canCp(getFocusTile(), action);

        return true;
    default:
        return true;
    }
}

///
/// \brief Process user input
///
void Table::process()
{
    // select out those actions with highest priority
    util::Stactor<Who, 4> actors;
    ActCode maxAct = ActCode::NOTHING;
    for (int w = 0; w < 4; w++) {
        if (mActionInbox[w].act() > maxAct) {
            actors.clear();
            maxAct = mActionInbox[w].act();
        }

        if (mActionInbox[w].act() == maxAct)
            actors.emplaceBack(w);
    }

    assert(!actors.empty());
    assert(maxAct != ActCode::NOTHING);

    if (maxAct == ActCode::END_TABLE) {
        endTable();
    } else if (maxAct == ActCode::NEXT_ROUND) {
        nextRound();
    } else if (maxAct == ActCode::RON) {
        if (!mRule.headJump && actors.size() == 3) {
            // no support for 3-ron yet (either headjump or exhaust)
            exhaustRound(RoundResult::SCHR, actors);
        } else {
            finishRound(actors, mFocus.who());
        }
    } else if (maxAct == ActCode::PASS) {
        everyonePassed();
    } else {
        assert(actors.size() == 1);
        singleAction(actors[0], mActionInbox[actors[0].index()]);
    }
}

///
/// \brief Process an action performed by only one player
/// \param who The acting player
/// \param act The action
///
void Table::singleAction(Who who, const Action &act)
{
    util::Stactor<Who, 4> openers = { who };

    switch (act.act()) {
    case ActCode::SWAP_OUT:
        swapOut(who, act.t37());
        break;
    case ActCode::SPIN_OUT:
        spinOut(who);
        break;
    case ActCode::SWAP_RIICHI:
    case ActCode::SPIN_RIICHI:
        declareRiichi(who, act);
        break;
    case ActCode::CHII_AS_LEFT:
    case ActCode::CHII_AS_MIDDLE:
    case ActCode::CHII_AS_RIGHT:
        chii(who, act.act(), act.t37(), act.showAka5() > 0);
        break;
    case ActCode::PON:
        pon(who, act.t37(), act.showAka5());
        break;
    case ActCode::DAIMINKAN:
        daiminkan(who);
        break;
    case ActCode::ANKAN:
        ankan(who, act.t34());
        break;
    case ActCode::KAKAN:
        kakan(who, act.barkId());
        break;
    case ActCode::TSUMO:
        finishRound(openers, Who());
        break;
    case ActCode::RYUUKYOKU:
        exhaustRound(RoundResult::KSKP, openers);
        break;
    case ActCode::DICE:
        rollDice();
        break;
    default:
        unreached("singleAction: unhandled act");
    }
}

///
/// \brief Setup the observers
/// \param obs Observers from the client code
///
void Table::setupObservers(const std::vector<TableObserver *> obs)
{
    // feature: girls are guaranteed to be the first observers
    //          because other observer may observe girls' state
    for (auto &g : mGirls)
        mObservers.push_back(g.get());

    mObservers.insert(mObservers.end(), obs.begin(), obs.end());
}

///
/// \brief Start a new round
///
void Table::nextRound()
{
    clean();

#ifdef LIBSAKI_CHEAT_ROUND
    mRound = cheat::round;
    mExtraRound = cheat::extra;
    mDealer = Who(cheat::dealer);
    mAllLast = cheat::allLast;
    mDeposit = cheat::deposit;
    mRand.set(cheat::state);
#else
    if (mToChangeDealer) {
        mDealer = mDealer.right();
        mRound++;
    }

    if (mRound + 1 >= mRule.roundLimit)
        mAllLast = true;

    // increase when renchan or HP
    mExtraRound = !mToChangeDealer || mPrevIsRyuu ? mExtraRound + 1 : 0;
#endif

    TableEvent event = TableEvent::RoundStarted {
        mRound, mExtraRound, mDealer,
        mAllLast, mDeposit, mRand.state()
    };

    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    mMount = Mount(mRule.akadora);

    mChoicess[mDealer.index()].setDice();
    for (auto &g : mGirls)
        g->onDice(mRand, *this);
}

///
/// \brief Clean up the table
///
void Table::clean()
{
    for (int w = 0; w < 4; w++) {
        mFuritens[w] = Furiten();
        mRivers[w].clear();
        mPickeds[w].reset();
        mGenbutsuFlags[w].reset();
    }

    mRiichiHans.fill(0);
    mLayPositions.fill(-1);

    mIppatsuFlags.reset();

    mToEstablishRiichi = false;
    mToFlip = false;

    mKanContext = KanCtx();
    mDice = 0; // zero to mark not rolled yet

    TableEvent event = TableEvent::Cleaned {};
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);
}

///
/// \brief Roll the dice, and enter the next stage
///
void Table::rollDice()
{
    int die1 = mRand.gen(6) + 1;
    int die2 = mRand.gen(6) + 1;

    if (beforeEast1()) {
        for (auto &g : mGirls)
            g->onChooseFirstDealer(mRand, mInitDealer, die1, die2);
    }

    mDice = die1 + die2;

    TableEvent event = TableEvent::Diced { die1, die2 };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    if (beforeEast1()) {
        mInitDealer = mInitDealer.byDice(mDice);
        mDealer = mInitDealer;

        TableEvent event = TableEvent::FirstDealerChosen { mInitDealer };
        for (auto ob : mObservers)
            ob->onTableEvent(*this, event);

        nextRound(); // enter first round
    } else {
        deal(); // open the mount
    }
}

///
/// \brief Give 14 or 13 initial tiles to the four players
///
void Table::deal()
{
    mHands = Princess(*this, mRand, mMount, mGirls).dealAndFlip();

    TableEvent event = TableEvent::Dealt {};
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    notifyFlipped();
    tryDraw(mDealer);
}

///
/// \brief Flip a new kandora indicator
///
void Table::flipKandoraIndic()
{
    mMount.flipIndic(mRand);
    notifyFlipped();
}

///
/// \brief Tell observers about the new dora indicator
///
void Table::notifyFlipped()
{
    TableEvent event = TableEvent::Flipped {};
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);
}

///
/// \brief Draw a tile from the mountain,
///        and triggers ryuukyoku if the mountain is empty
/// \param who The player receiving the tile
///
void Table::tryDraw(Who who)
{
    // SCRC
    if (mToEstablishRiichi)
        if (!finishRiichi())
            return;

    bool rinshan = mKanContext.during();
    int w = who.index();

    if (mMount.remainPii() > 0) {
        for (auto &g : mGirls)
            g->onDraw(*this, mMount, who, rinshan);

        T37 tile = mMount.pop(mRand, rinshan);
        mHands[w].draw(tile);

        Choices::ModeDrawn mode;

        mode.swapOut = !riichiEstablished(who);
        mode.tsumo = mHands[w].canTsumo(getFormCtx(who), mRule);
        mode.kskp = noBarkYet() && mRivers[w].empty() && mHands[w].nine9();

        if (mMount.remainPii() >= 4
            && mPoints[w] >= 1000
            && !riichiEstablished(who)) {
            mHands[w].canRiichi(mode.swapRiichis, mode.spinRiichi);
        }

        // must be able to maintain king-14, and forbid 5th kan
        if (mMount.remainPii() > 0 && mMount.remainRinshan() > int(mToFlip)) {
            mHands[w].canAnkan(mode.ankans, riichiEstablished(who));
            mHands[w].canKakan(mode.kakans);
        }

        mChoicess[w].setDrawn(mode);

        TableEvent event = TableEvent::Drawn { who };
        for (auto ob : mObservers)
            ob->onTableEvent(*this, event);
    } else { // wall empty
        assert(!rinshan);

        util::Stactor<Who, 4> swimmers;
        for (int w = 0; w < 4; w++)
            if (checkNagashimangan(Who(w)))
                swimmers.emplaceBack(w);

        if (mRule.nagashimangan && !swimmers.empty()) {
            exhaustRound(RoundResult::NGSMG, swimmers);
        } else {
            util::Stactor<Who, 4> openers;
            for (int w = 0; w < 4; w++)
                if (mHands[w].ready())
                    openers.emplaceBack(w);

            exhaustRound(RoundResult::HP, openers);
        }
    }
}

///
/// \brief Performs discarding in a swap-out action
/// \param who The swapping-out player
/// \param out The tile to be swapped-out
///
void Table::swapOut(Who who, const T37 &out)
{
    mKanContext.leave();

    mRivers[who.index()].pushBack(out);
    mHands[who.index()].swapOut(out);

    discardEffects(who, false);
}

///
/// \brief Performs discarding in a spin-out action
/// \param who The spinning-out player
///
void Table::spinOut(Who who)
{
    mKanContext.leave();

    const T37 &out = mHands[who.index()].drawn();
    mRivers[who.index()].pushBack(out);
    mHands[who.index()].spinOut();

    discardEffects(who, true);
}

///
/// \brief Performs discarding in a bark action
/// \param who The barking player
/// \param out The discarding tile
///
void Table::barkOut(Who who, const T37 &out)
{
    mRivers[who.index()].pushBack(out);
    mHands[who.index()].barkOut(out);

    discardEffects(who, false);
}

///
/// \brief Deal with matters when a discard happens
/// \param who The player who discarded
/// \param spin true if spin, false if swap
///
void Table::discardEffects(Who who, bool spin)
{
    mFocus.focusOnDiscard(who);

    TableEvent event = TableEvent::Discarded { spin };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    mIppatsuFlags.reset(mFocus.who().index());
    mFuritens[mFocus.who().index()].doujun = false;

    if (checkDeathWinds()) {
        util::Stactor<Who, 4> openers; // those who wasted 1000 points
        for (int w = 0; w < 4; w++)
            if (riichiEstablished(Who(w)))
                openers.emplaceBack(w);

        exhaustRound(RoundResult::SFRT, openers);
        return;
    }

    if (mToFlip) {
        mToFlip = false;
        flipKandoraIndic();
    }

    mGenbutsuFlags[mFocus.who().index()].set(getFocusTile().id34());
    for (int w = 0; w < 4; w++)
        if (riichiEstablished(Who(w)))
            mGenbutsuFlags[w].set(getFocusTile().id34());

    checkSutehaiFuriten();
    checkBarkRon();

    if (!anyActivated())
        tryDraw(mFocus.who().right());
}

///
/// \brief Performs one's riichi declaration
/// \param who The player who declares the riichi
/// \param action Either SWAP_RIICHI or SPIN_RIICHI
///
void Table::declareRiichi(Who who, const Action &action)
{
    mToEstablishRiichi = true;
    mLayPositions[who.index()] = mRivers[who.index()].size();

    TableEvent event = TableEvent::RiichiCalled { who };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    if (action.act() == ActCode::SWAP_RIICHI)
        swapOut(who, action.t37());
    else if (action.act() == ActCode::SPIN_RIICHI)
        spinOut(who);
    else
        unreached("Table::declareRiichi: non-riichi action");
}

///
/// \brief Deal with matters when a riichi is established
/// \return true if ok, false on the 4-riichi problem (SCRC)
///
bool Table::finishRiichi()
{
    assert(mFocus.isDiscard());
    int w = mFocus.who().index();
    mPoints[w] -= 1000;
    mDeposit += 1000;
    mToEstablishRiichi = false;
    mRiichiHans[w] = noBarkYet() && mRivers[w].size() == 1 ? 2 : 1;
    mIppatsuFlags.set(w);

    TableEvent eventPoints = TableEvent::PointsChanged {};
    TableEvent eventRiichi = TableEvent::RiichiEstablished { mFocus.who() };
    for (auto ob : mObservers) {
        ob->onTableEvent(*this, eventPoints);
        ob->onTableEvent(*this, eventRiichi);
    }

    if (util::all(mRiichiHans, [](int han) { return han != 0; })) {
        util::Stactor<Who, 4> openers { Who(0), Who(1), Who(2), Who(3) };
        exhaustRound(RoundResult::SCRC, openers);
        return false;
    }

    return true;
}

///
/// \brief Make a chii happen
/// \param who The player to chii
/// \param dir The type of chii
/// \param out The tile to discard in this chii
/// \param showAka5 Maximal number of akadoro to show out
///
void Table::chii(Who who, ActCode dir, const T37 &out, bool showAka5)
{
    // save typing
    const ActCode L = ActCode::CHII_AS_LEFT;
    const ActCode M = ActCode::CHII_AS_MIDDLE;
    const ActCode R = ActCode::CHII_AS_RIGHT;

    assert(who == mFocus.who().right());
    assert(dir == L || dir == M || dir == R);

    pick();

    void (Hand::*pChii) (const T37 &pick, bool showAka5);
    pChii = dir == L ? &Hand::chiiAsLeft
                     : dir == M ? &Hand::chiiAsMiddle : &Hand::chiiAsRight;

    (mHands[who.index()].*pChii)(getFocusTile(), showAka5);

    TableEvent event = TableEvent::Barked { who, mHands[who.index()].barks().back(), false };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    barkOut(who, out);
}

///
/// \brief Make a pon happen
/// \param who The player to pon
/// \param out The discarded tile in this pon
/// \param showAka5 Maximal number of akadoro to show out
///
void Table::pon(Who who, const T37 &out, int showAka5)
{
    pick();

    int layIndex = who.looksAt(mFocus.who());
    mHands[who.index()].pon(getFocusTile(), showAka5, layIndex);

    TableEvent event = TableEvent::Barked { who, mHands[who.index()].barks().back(), false };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    barkOut(who, out);
}

///
/// \brief Make a daiminkan happen
/// \param who The player to daiminkan
///
void Table::daiminkan(Who who)
{
    mKanContext.enterDaiminkan(mFocus.who());

    pick();

    int layIndex = who.looksAt(mFocus.who());
    mHands[who.index()].daiminkan(getFocusTile(), layIndex);

    TableEvent event = TableEvent::Barked { who, mHands[who.index()].barks().back(), false };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    finishKan(who);
}

///
/// \brief Mark the last-discarded tile as picked
///
/// The tile object still presents in the river after picked.
///
void Table::pick()
{
    assert(mFocus.isDiscard());

    if (mToEstablishRiichi)
        finishRiichi();

    int pickee = mFocus.who().index();
    mPickeds[pickee][mRivers[pickee].size() - 1] = true;
    if (lastDiscardLay())
        mLayPositions[pickee]++;

    mIppatsuFlags.reset(); // AoE ippatsu canceling
}

///
/// \brief Make an ankan happen
/// \param who The player to ankan
/// \param tile The tiles forming the new ankan
///
void Table::ankan(Who who, T34 tile)
{
    mKanContext.enterAnkan();

    mIppatsuFlags.reset(); // AoE ippatsu canceling

    if (mToFlip) { // consecutive kans
        mToFlip = false;
        flipKandoraIndic();
    }

    int w = who.index();
    bool spin = mHands[w].drawn() == tile;
    mHands[w].ankan(tile);
    mFocus.focusOnChankan(who, mHands[who.index()].barks().size() - 1);

    TableEvent event = TableEvent::Barked { who, mHands[who.index()].barks().back(), spin };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    checkChankan(true);

    if (!anyActivated())
        finishKan(who);
}

///
/// \brief Make a kakan happen
/// \param who The player to kakan
/// \param barkId The index of the pon that will become a kakan
///
void Table::kakan(Who who, int barkId)
{
    mKanContext.enterKakan();

    mIppatsuFlags.reset(); // AoE ippatsu canceling

    if (mToFlip) { // consecutive kans
        mToFlip = false;
        flipKandoraIndic();
    }

    int w = who.index();
    bool spin = mHands[w].drawn() == mHands[w].barks()[barkId][0];

    mHands[w].kakan(barkId);
    mFocus.focusOnChankan(who, barkId);
    const M37 &kanMeld = mHands[who.index()].barks()[barkId];

    TableEvent event = TableEvent::Barked { who, kanMeld, spin };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);

    checkChankan();

    if (!anyActivated())
        finishKan(who);
}

///
/// \brief Deal with various matters when a kan finished
/// \param who The kanned player
///
void Table::finishKan(Who who)
{
    if (kanOverflow(who)) {
        util::Stactor<Who, 4> openers;
        for (int w = 0; w < 4; w++)
            if (riichiEstablished(Who(w)))
                openers.emplaceBack(w);

        exhaustRound(RoundResult::SKSR, openers);
        return;
    }

    if (mRule.kandora) {
        if (mKanContext.duringMinkan())
            mToFlip = true;
        else
            flipKandoraIndic();
    }

    tryDraw(who);
}

///
/// \brief Activate players who need to act
///
void Table::activate()
{
    mActionInbox.fill(Action()); // clear

    if (!filterChoices())
        return;

    auto isDice = [](const Choices &c) { return c.mode() == Choices::Mode::DICE; };
    if (util::any(mChoicess, isDice))
        for (auto &g : mGirls)
            g->onActivateDice(*this);

    for (int w = 0; w < 4; w++)
        if (mChoicess[w].any())
            mGirls[w]->onActivate(*this);
}

///
/// \brief Process the case when everyone pressed the 'pass' button
///
void Table::everyonePassed()
{
    if (mFocus.isDiscard()) { // pass a bark or an ordinary ron
        tryDraw(mFocus.who().right());
    } else { // pass a chankan
        assert(mKanContext.during());
        finishKan(mFocus.who());
    }
}

///
/// \brief Performs the activation-filtering stage
/// \return true iff still has actors after filtering
///
bool Table::filterChoices()
{
    auto isBark = [](const Choices &c) { return c.mode() == Choices::Mode::BARK; };
    bool hadBarker = util::any(mChoicess, isBark);

    for (Who actor : whos::ALL4) {
        Choices &choices = mChoicess[actor.index()];
        if (choices.any()) {
            bool couldRon = choices.can(ActCode::RON);

            ChoiceFilter filter;
            for (Who f : whos::ALL4)
                mGirls[f.index()]->onFilterChoice(*this, actor, filter);

            choices.filter(filter);

            if (couldRon && !choices.can(ActCode::RON))
                passRon(actor);
        }
    }

    if (hadBarker && !anyActivated()) {
        everyonePassed();
        return false;
    }

    return true;
}

///
/// \brief Checks the 4-kan problem before 'kanner' finish a kan
/// \param kanner The kanning player
/// \return true iff round aborted
///
bool Table::kanOverflow(Who kanner)
{
    // sksr happens befor3e rinshan, so +1 in advance
    int kanCt = (4 - mMount.remainRinshan()) + 1;

    if (kanCt == 5)
        return true;

    if (kanCt == 4) {
        const auto &barks = mHands[kanner.index()].barks();
        auto isKan = [](const M37 &m) { return m.isKan(); };
        return !(barks.size() == 4 && util::all(barks, isKan));
    }

    return false;
}

///
/// \brief Checks if no bark has happened this round
/// \return true iff no bark
///
bool Table::noBarkYet() const
{
    auto empty = [](const Hand &h) { return h.barks().empty(); };
    return util::all(mHands, empty);
}

///
/// \brief Checks the 4-wind problem (SFRT)
/// \return true iff round aborted
///
bool Table::checkDeathWinds() const
{
    if (!noBarkYet())
        return false;

    auto notOne = [](const River &r) { return r.size() != 1; };
    if (util::any(mRivers, notOne))
        return false;

    if (getFocusTile().suit() != Suit::F)
        return false;

    const auto &r = mRivers; // save typing
    return r[0][0] == r[1][0] && r[1][0] == r[2][0] && r[2][0] == r[3][0];
}

///
/// \brief Checks for chankan
/// \param only13 false in common cases, true to consider kokushimusou only
///
void Table::checkChankan(bool only13)
{
    for (int w = 0; w < 4; w++) {
        if (Who(w) == mFocus.who())
            continue;

        if (mFuritens[w].none() && (!only13 || mHands[w].step13() == 0)) {
            Choices::ModeBark mode;
            mode.focus = getFocusTile();
            bool passiveDoujun = false;
            mode.ron = mHands[w].canRon(getFocusTile(), getFormCtx(Who(w)), mRule, passiveDoujun);
            assert(!passiveDoujun);
            if (mode.ron)
                mChoicess[w].setBark(mode);
        }
    }
}

///
/// \brief Checks chii, pon, daiminkan, and ron after discarding
///
void Table::checkBarkRon()
{
    const T37 &focus = getFocusTile();

    for (int w = 0; w < 4; w++) {
        if (Who(w) == mFocus.who())
            continue;

        Choices::ModeBark mode;
        mode.focus = focus;

        // ron
        if (mFuritens[w].none()) {
            bool passiveDoujun = false;
            mode.ron = mHands[w].canRon(focus, getFormCtx(Who(w)), mRule, passiveDoujun);
            mFuritens[w].doujun = mFuritens[w].doujun || passiveDoujun;
        }

        // bark
        if (mMount.remainPii() > 0 && !riichiEstablished(Who(w))) {
            mode.pon = mHands[w].canPon(focus);
            mode.dmk = mMount.remainRinshan() > 0 && mHands[w].canDaiminkan(focus);
            if (Who(w) == mFocus.who().right()) {
                mode.chiiL = mHands[w].canChiiAsLeft(focus);
                mode.chiiM = mHands[w].canChiiAsMiddle(focus);
                mode.chiiR = mHands[w].canChiiAsRight(focus);
            }

            if (mode.pon || mode.dmk || mode.chiiL || mode.chiiM || mode.chiiR)
                mode.swapBarks = mHands[w].closed().t37s13();
        }

        if (mode.any())
            mChoicess[w].setBark(mode);
    }
}

///
/// \brief Checks sutehai furiten after discarding
///
void Table::checkSutehaiFuriten()
{
    Who dropper = mFocus.who();
    int w = dropper.index();

    if (!mHands[w].ready())
        return;

    for (T34 t : mHands[w].effA()) {
        if (util::any(mRivers[w], [t](const T37 &r) { return r == t; })) {
            mFuritens[w].sutehai = true;
            return;
        }
    }

    mFuritens[w].sutehai = false;
}

///
/// \brief Process the case when a player passed a ron chance
/// \param who The player passed ron
///
void Table::passRon(Who who)
{
    if (riichiEstablished(who))
        mFuritens[who.index()].riichi = true;
    else
        mFuritens[who.index()].doujun = true;
}

///
/// \brief Checks if one's river forms nagashimangan
/// \param who The owner of the river to check
/// \return true iff nagashimangan confirmed
///
bool Table::checkNagashimangan(Who who) const
{
    int w = who.index();
    return util::all(mRivers[w], [](const T37 &t) { return t.isYao(); })
           && mPickeds[w].none();
}

///
/// \brief Make a ryuukyoku happen
/// \param result Type of ryuukyoku
/// \param openers Players who need to open their hand
///
void Table::exhaustRound(RoundResult result, const util::Stactor<Who, 4> &openers)
{
    if (result == RoundResult::HP) {
        std::array<bool, 4> tenpai { false, false, false, false };
        for (Who who : openers)
            tenpai[who.index()] = true;

        int ct = openers.size();
        if (ct % 4 != 0) {
            for (int w = 0; w < 4; w++)
                mPoints[w] += tenpai[w] ? (3000 / ct) : -(3000 / (4 - ct));
        }

        mToChangeDealer = !util::has(openers, mDealer);
    } else if (result == RoundResult::NGSMG) {
        for (Who who : openers) {
            if (who == mDealer)
                for (int l = 0; l < 4; ++l)
                    mPoints[l] += l == who.index() ? 12000 : -4000;

            else
                for (int l = 0; l < 4; ++l)
                    mPoints[l] += l == who.index() ? 8000
                                                   : -(l == mDealer.index() ? 4000 : 2000);
        }

        mToChangeDealer = !util::has(openers, mDealer);
    } else { // abortive round endings
        if (result == RoundResult::KSKP && mToEstablishRiichi)
            finishRiichi();

        mToChangeDealer = false;
    }

    util::Stactor<Form, 2> forms;
    TableEvent eventEnd = TableEvent::RoundEnded { result, openers, Who(), forms };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, eventEnd);

    TableEvent eventPoints = TableEvent::PointsChanged {};
    for (auto ob : mObservers)
        ob->onTableEvent(*this, eventPoints);

    checkNextRound();
    mPrevIsRyuu = true;
}

///
/// \brief Make a tsumo or ron happen
/// \param openers_ Players who need to open their hands
/// \param gunner Loser in a ron case
///
void Table::finishRound(const util::Stactor<Who, 4> &openers_, Who gunner)
{
    util::Stactor<Who, 4> openers(openers_); // non-const copy
    assert(!openers.empty());

    bool isRon = gunner.somebody();

    if (isRon && openers.size() >= 2) {
        // *INDENT-OFF*
        auto compDist = [gunner](Who w1, Who w2) {
            return w1.turnFrom(gunner) < w2.turnFrom(gunner);
        };
        // *INDENT-ON*
        std::sort(openers.begin(), openers.end(), compDist);
    }

    int ticket = mRule.headJump ? 1 : openers.size();
    assert(ticket == 1 || ticket == 2); // no '3-ron' yet

    // dig uradora-indicator if some winner established riichi
    auto est = [this](Who who) { return riichiEstablished(who); };
    if (mRule.uradora && util::any(openers.begin(), openers.begin() + ticket, est))
        mMount.digIndic(mRand);

    util::Stactor<Form, 2> forms;
    for (int i = 0; i < ticket; i++) {
        Who who = openers[i];
        int w = who.index();
        if (isRon) {
            forms.emplaceBack(mHands[w], getFocusTile(), getFormCtx(who), mRule,
                              mMount.getDrids(), mMount.getUrids());
        } else {
            forms.emplaceBack(mHands[w], getFormCtx(who), mRule,
                              mMount.getDrids(), mMount.getUrids());
        }
    }

    TableEvent eventEnd = TableEvent::RoundEnded {
        isRon ? RoundResult::RON : RoundResult::TSUMO,
        openers, gunner, forms
    };

    for (auto ob : mObservers)
        ob->onTableEvent(*this, eventEnd);

    // the closest player (or the tsumo player) takes deposit
    mPoints[openers[0].index()] += mDeposit;
    mDeposit = 0;

    // pay the form value
    if (isRon) {
        for (size_t i = 0; i < forms.size(); i++) {
            mPoints[gunner.index()] -= forms[i].loss(false); // 'false' unused
            mPoints[openers[i].index()] += forms[i].gain();
        }
    } else if (mRule.daiminkanPao
               && mKanContext.during()
               && mKanContext.pao().somebody()) {
        int loss = 2 * forms[0].loss(false) + forms[0].loss(true);
        mPoints[mKanContext.pao().index()] -= loss;
        mPoints[openers[0].index()] += forms[0].gain();
    } else { // tsumo
        for (int i = 0; i < 4; ++i) {
            Who who(i);
            if (who == openers[0])
                mPoints[i] += forms[0].gain();
            else
                mPoints[i] -= forms[0].loss(who == mDealer);
        }
    }

    TableEvent eventPoints = TableEvent::PointsChanged {};
    for (auto ob : mObservers)
        ob->onTableEvent(*this, eventPoints);

    mToChangeDealer = !util::has(openers.begin(), openers.begin() + ticket, mDealer);
    checkNextRound();
    mPrevIsRyuu = false;
}

///
/// \brief Checks if next round is available
///
void Table::checkNextRound()
{
    bool fly = mRule.fly && util::any(mPoints, [](int p) { return p < 0; });
    bool hasWest = util::all(mPoints, [&](int p) { return p <= mRule.returnLevel; });
    bool topDealer = getRank(mDealer) == 1;
    bool end = fly || (mAllLast && !hasWest && (mToChangeDealer || topDealer));

    for (int w = 0; w != 4; w++) {
        bool flog = Who(w) == mDealer && !fly && mAllLast && !mToChangeDealer && topDealer;
        Choices::ModeEnd mode;
        mode.end = end;
        mode.next = !end || flog;
        mChoicess[w].setEnd(mode);
    }
}

///
/// \brief End up the table and calculate final points
///
void Table::endTable()
{
    std::array<Who, 4> rank;
    for (int w = 0; w != 4; ++w)
        rank[getRank(Who(w)) - 1] = Who(w);

    mPoints[rank[0].index()] += mDeposit;

    std::array<int, 4> scores(mPoints);
    scores[rank[0].index()] += mRule.hill;

    for (int &sc : scores) {
        sc -= mRule.returnLevel;
        int q = std::abs(sc) / 1000;
        int r = std::abs(sc) % 1000;
        if (r >= 600)
            q++;

        sc = sc > 0 ? q : -q;
    }

    TableEvent event = TableEvent::TableEnded { rank, scores };
    for (auto ob : mObservers)
        ob->onTableEvent(*this, event);
}



} // namespace saki
