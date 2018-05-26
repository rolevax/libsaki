#include "replay.h"
#include "../table/table.h"
#include "../util/misc.h"



// *INDENT-OFF*
//
// PANCAKE MAHJONG PLAY RECORD DOCUMENT
// VERSION 3
//
// {
//     "version": 3
//     "datetime": 20160926161923
//     "girls": [ 1001, 1002, 2001, 2002 ]
//     "init_points": [ 25000, 25000, 25000, 25000 ]
//     "scores": [ 45, 10, -15, -40 ]
//     "rule": { ... }
//     "rounds": [
//         {
//             "round": 0
//             "extraRound": 0
//             "dealer": 0
//             "allLast": false
//             "deposit": 0
//             "state": 2333333
//             "die1": 3
//             "die2": 5
//             "drids": [ "1f" ]
//             "urids": [ "2f" ]
//             "tracks": [
//                 {
//                     "init": [ "1m", "2m", "3m", ... ]
//                     "in": [ "1m", "2m", "3m", ... ]
//                     "out": [ "1m", "->", "3m", ... ]
//                 },
//                 {
//                     "init": [ "1m", "2m", "3m", ... ]
//                     "in": [ "1m", "2m", "3m", ... ]
//                     "out": [ "1m", "2m", "3m", ... ]
//                 },
//                 ...
//             ]
//         }
//     ]
// }
//
// chii: "1s23", "2s13", "3s12"
// pon/daiminkan: "5p05", "0p55", "0p055"
// ankan: "a5p"
// kakan: "k0p", "k5p"
//
// *INDENT-ON*



namespace saki
{



void Replay::onTableEvent(const Table &table, const TE::TableStarted &event)
{
    for (int w = 0; w < 4; w++)
        girls[w] = table.getGirl(Who(w)).getId();

    initPoints = table.getPoints();
    rule = table.getRule();
    seed = event.seed;
}

void Replay::onTableEvent(const Table &table, const TableEvent::RoundStarted &event)
{
    (void) table;

    rounds.emplace_back();

    rounds.back().round = event.round;
    rounds.back().extraRound = event.extraRound;
    rounds.back().dealer = event.dealer;
    rounds.back().allLast = event.allLast;
    rounds.back().deposit = event.deposit;
    rounds.back().state = event.seed;
}

void Replay::onTableEvent(const Table &table, const TE::Diced &event)
{
    (void) table;

    if (!rounds.empty()) { // else init-dealer choosing dice
        rounds.back().die1 = event.die1;
        rounds.back().die2 = event.die2;
    }
}

void Replay::onTableEvent(const Table &table, const TE::Dealt &event)
{
    (void) event;

    for (int w = 0; w < 4; w++) {
        const auto &ts = table.getHand(Who(w)).closed().t37s13(true);
        assert(ts.size() == 13);
        for (int i = 0; i < 13; i++)
            rounds.back().tracks[w].init[i] = ts[i];
    }
}

void Replay::onTableEvent(const Table &table, const TE::Flipped &event)
{
    (void) event;

    const T37 &newIndic = table.getMount().getDrids().back();
    rounds.back().drids.emplace_back(newIndic);
}

void Replay::onTableEvent(const Table &table, const TE::Drawn &event)
{
    const T37 &drawn = table.getHand(event.who).drawn();
    rounds.back().tracks[event.who.index()].in.emplace_back(In::DRAW, drawn);
}

void Replay::onTableEvent(const Table &table, const TE::Discarded &event)
{
    Who who = table.getFocus().who();

    Out act;
    if (mToEstablishRiichi) {
        mToEstablishRiichi = false;
        act = event.spin ? Out::RIICHI_SPIN : Out::RIICHI_ADVANCE;
    } else {
        act = event.spin ? Out::SPIN : Out::ADVANCE;
    }

    if (event.spin)
        rounds.back().tracks[who.index()].out.emplace_back(act);
    else
        rounds.back().tracks[who.index()].out.emplace_back(act, table.getFocusTile());
}

void Replay::onTableEvent(const Table &table, const TE::RiichiCalled &event)
{
    (void) table;
    (void) event;

    mToEstablishRiichi = true;
}

void Replay::onTableEvent(const Table &table, const TE::Barked &event)
{
    int w = event.who.index();
    const auto &bark = event.bark;

    if (bark.isCpdmk())
        addSkip(event.who, table.getFocus().who());

    if (bark.type() == M37::Type::CHII) {
        bool showAka5 = false;
        size_t lay = bark.layIndex();

        for (size_t i = 0; i < bark.tiles().size(); i++) {
            if (i != lay && bark.tiles().at(i).isAka5()) {
                showAka5 = true;
                break;
            }
        }

        std::array<In, 3> chiis { In::CHII_AS_LEFT, In::CHII_AS_MIDDLE, In::CHII_AS_RIGHT };
        In act = chiis[lay];

        rounds.back().tracks[w].in.emplace_back(act, showAka5);
    } else if (bark.type() == M37::Type::PON) {
        int showAka5 = 0;
        for (int i = 0; i < static_cast<int>(bark.tiles().size()); i++)
            showAka5 += (i != bark.layIndex() && bark.tiles().at(i).isAka5());

        rounds.back().tracks[w].in.emplace_back(In::PON, showAka5);
    } else if (bark.type() == M37::Type::DAIMINKAN) {
        rounds.back().tracks[w].in.emplace_back(In::DAIMINKAN);
        rounds.back().tracks[w].out.emplace_back(Out::SKIP_OUT);
    } else if (bark.type() == M37::Type::ANKAN) {
        T37 t(bark[0].id34()); // use id34 for ankan
        rounds.back().tracks[w].out.emplace_back(Out::ANKAN, t);
    } else if (bark.type() == M37::Type::KAKAN) {
        rounds.back().tracks[w].out.emplace_back(Out::KAKAN, bark[3]);
    }
}

void Replay::onTableEvent(const Table &table, const TE::RoundEnded &event)
{
    rounds.back().result = event.result;

    for (const T37 &t : table.getMount().getUrids())
        rounds.back().urids.emplace_back(t);

    for (const auto &form : event.forms) {
        rounds.back().spells.emplace_back(form.spell());
        rounds.back().charges.emplace_back(form.charge());
    }

    if (event.result == RoundResult::KSKP) {
        rounds.back().tracks[event.openers[0].index()].out.emplace_back(Out::RYUUKYOKU);
    } else if (event.result == RoundResult::TSUMO) {
        rounds.back().tracks[event.openers[0].index()].out.emplace_back(Out::TSUMO);
    } else if (event.result == RoundResult::RON || event.result == RoundResult::SCHR) {
        // no need to add skip from gunner because there is nothing after ron
        for (Who who = event.gunner.right(); who != event.gunner; who = who.right()) {
            if (util::has(event.openers, who)) {
                rounds.back().tracks[who.index()].in.emplace_back(In::RON);
            } else { // not an opener, turn-fly
                rounds.back().tracks[who.index()].in.emplace_back(In::SKIP_IN);
                rounds.back().tracks[who.index()].out.emplace_back(Out::SKIP_OUT);
            }
        }
    }
}

void Replay::onTableEvent(const Table &table, const TE::PointsChanged &event)
{
    (void) event;

    rounds.back().resultPoints = table.getPoints();
}

TableSnap Replay::look(int roundId, int turn)
{
    TableSnap snap;
    std::array<TileCount, 4> hands;
    const Round &round = rounds[roundId];
    const std::array<Track, 4> &tracks = round.tracks;

    snap.round = round.round;
    snap.extraRound = round.extraRound;
    snap.dealer = round.dealer;
    snap.allLast = round.allLast;
    snap.deposit = round.deposit;
    snap.state = round.state;
    snap.die1 = round.die1;
    snap.die2 = round.die2;
    snap.result = round.result;
    snap.points = roundId == 0 ? initPoints : rounds[roundId - 1].resultPoints;

    // deal stage
    for (int w = 0; w < 4; w++)
        for (const T37 &t : tracks[w].init)
            hands[w].inc(t, 1);

    snap.wallRemain = 70;
    snap.deadRemain = 4;

    // flip first indicator
    if (!round.drids.empty())
        snap.drids.pushBack(round.drids[0]);

    Who who = round.dealer;
    auto next = [&who]() { who = who.right(); };
    std::array<int, 4> steps = { 0, 0, 0, 0 };
    bool toRiichi = false;
    bool toFlip = false;
    bool kanContext = false;
    Who lastDiscarder;

    for (bool inStage = true; turn-- > 0; inStage = !inStage) {
        int step = steps[who.index()];

        // in-stage
        if (inStage) {
            if (step >= int(tracks[who.index()].in.size()))
                break;

            const InAct &in = tracks[who.index()].in[step];

            if (toRiichi && in.act != In::RON) {
                toRiichi = false;
                snap[lastDiscarder.index()].riichiBar = true;
                snap.points[lastDiscarder.index()] -= 1000;
            }

            switch (in.act) {
            case In::DRAW:
                snap.whoDrawn = who;
                snap.drawn = in.t37;
                snap.wallRemain--;
                if (kanContext)
                    snap.deadRemain--;

                break;
            case In::CHII_AS_LEFT:
            case In::CHII_AS_MIDDLE:
            case In::CHII_AS_RIGHT:
                lookChii(snap, hands[who.index()], in, who, lastDiscarder);
                break;
            case In::PON:
                lookPon(snap, hands[who.index()], in.showAka5, who, lastDiscarder);
                break;
            case In::DAIMINKAN:
                kanContext = true;
                lookDaiminkan(snap, hands[who.index()], who, lastDiscarder);
                toFlip = true;
                break;
            case In::RON:
                snap.endOfRound = true;
                snap.openers.emplace_back(who);

                if (kanContext) {
                    snap.cannon = snap[snap.gunner.index()].barks.back()[3];
                } else {
                    snap.cannon = snap[snap.gunner.index()].river.back();
                }

                [[fallthrough]];
            case In::SKIP_IN:
                turn++; // no consume
                inStage = !inStage; // stay in in-stage
                steps[who.index()]++;
                next();
                break;
            }
        } else { // out-stage
            if (step >= int(tracks[who.index()].out.size()))
                break;

            const OutAct &out = tracks[who.index()].out[step];

            steps[who.index()]++;

            // *INDENT-OFF*
            auto checkFlip = [&snap, &round, &toFlip]() {
                if (toFlip && snap.drids.size() < round.drids.size()) {
                    snap.drids.pushBack(round.drids[snap.drids.size()]);
                    toFlip = false;
                }
            };
            // *INDENT-ON*

            // assumption: snap.gunner is only read after tsumo or ron
            snap.gunner = out.act == Out::TSUMO ? Who() : who;

            switch (out.act) {
            case Out::ADVANCE:
                kanContext = false;
                lookAdvance(snap, hands[who.index()], out.t37, who);
                lastDiscarder = who;
                checkFlip();
                next();
                break;
            case Out::SPIN:
                kanContext = false;
                snap[who.index()].river.emplace_back(snap.drawn);
                snap.whoDrawn = Who();
                lastDiscarder = who;
                checkFlip();
                next();
                break;
            case Out::RIICHI_ADVANCE:
                kanContext = false;
                toRiichi = true;
                snap[who.index()].riichiPos = snap[who.index()].river.size();
                lookAdvance(snap, hands[who.index()], out.t37, who);
                lastDiscarder = who;
                checkFlip();
                next();
                break;
            case Out::RIICHI_SPIN:
                kanContext = false;
                toRiichi = true;
                snap[who.index()].riichiPos = snap[who.index()].river.size();
                snap[who.index()].river.emplace_back(snap.drawn);
                snap.whoDrawn = Who();
                lastDiscarder = who;
                checkFlip();
                next();
                break;
            case Out::ANKAN:
                lookAnkan(snap, hands[who.index()], out.t37, who);
                kanContext = true;
                checkFlip(); // flipping of previous kan
                if (snap.drids.size() < round.drids.size()) // flip this kan
                    snap.drids.push_back(round.drids[snap.drids.size()]);

                break;
            case Out::KAKAN:
                lookKakan(snap, hands[who.index()], out.t37, who);
                kanContext = true;
                checkFlip(); // flipping of previous kan
                toFlip = true; // flip this kan
                break;
            case Out::RYUUKYOKU:
                snap.endOfRound = true;
                break;
            case Out::TSUMO:
                snap.endOfRound = true;
                snap.openers.emplace_back(who);
                break;
            case Out::SKIP_OUT:
                // daiminkan case only
                // the turn-fly case is handled in the in-stage
                turn++; // no consume
                break;
            }
        }
    }

    for (int w = 0; w < 4; w++)
        snap[w].hand = hands[w].t37s13(true);

    if (turn > 0) // exit before consumed all turns, means an abort
        snap.endOfRound = true;

    if (snap.endOfRound) {
        snap.spells = round.spells;
        snap.charges = round.charges;
        snap.points = round.resultPoints;
        for (const T37 &t : round.urids)
            snap.urids.pushBack(t);
    }

    return snap;
}

void Replay::addSkip(Who who, Who fromWhom)
{
    for (Who skippee = fromWhom.right(); skippee != who; skippee = skippee.right()) {
        int s = skippee.index();
        rounds.back().tracks[s].in.emplace_back(In::SKIP_IN);
        rounds.back().tracks[s].out.emplace_back(Out::SKIP_OUT);
    }
}

void Replay::lookAdvance(TableSnap &snap, TileCount &hand, const T37 &t37, Who who)
{
    snap[who.index()].river.emplace_back(t37);
    hand.inc(t37, -1);
    if (snap.whoDrawn.somebody()) {
        hand.inc(snap.drawn, 1);
        snap.whoDrawn = Who();
    }
}

void Replay::lookChii(TableSnap &snap, TileCount &hand, const InAct &in,
                      Who who, Who lastDiscarder)
{
    T37 pick = snap[lastDiscarder.index()].river.back();
    T37 t1, t2;
    if (in.act == In::CHII_AS_LEFT) {
        t1 = T37(pick.next().id34());
        t2 = T37(pick.nnext().id34());
    } else if (in.act == In::CHII_AS_MIDDLE) {
        t1 = T37(pick.prev().id34());
        t2 = T37(pick.next().id34());
    } else {
        assert(in.act == In::CHII_AS_RIGHT);
        t1 = T37(pick.pprev().id34());
        t2 = T37(pick.prev().id34());
    }

    if (in.showAka5) {
        assert(t1.val() == 5 || t2.val() == 5); // since so recorded
        if (t1.val() == 5)
            t1 = t1.toAka5();
        else if (t2.val() == 5)
            t2 = t2.toAka5();
    }

    if (in.act == In::CHII_AS_LEFT)
        snap[who.index()].barks.pushBack(M37::chii(pick, t1, t2, 0));
    else if (in.act == In::CHII_AS_MIDDLE)
        snap[who.index()].barks.pushBack(M37::chii(t1, pick, t2, 1));
    else // in.act == In::CHII_AS_RIGHT
        snap[who.index()].barks.pushBack(M37::chii(t1, t2, pick, 2));

    hand.inc(t1, -1);
    hand.inc(t2, -1);
    snap[lastDiscarder.index()].river.pop_back();
}

void Replay::lookPon(TableSnap &snap, TileCount &hand, int showAka5,
                     Who who, Who lastDiscarder)
{
    T37 pick = snap[lastDiscarder.index()].river.back();
    T37 t1(pick.id34());
    T37 t2(pick.id34());
    if (showAka5 >= 1)
        t1 = t1.toAka5();

    if (showAka5 >= 2)
        t2 = t2.toAka5();

    int openIndex = who.looksAt(lastDiscarder);
    if (openIndex == 0)
        snap[who.index()].barks.pushBack(M37::pon(pick, t1, t2, openIndex));
    else if (openIndex == 1)
        snap[who.index()].barks.pushBack(M37::pon(t1, pick, t2, openIndex));
    else // openIndex == 2
        snap[who.index()].barks.pushBack(M37::pon(t1, t2, pick, openIndex));

    hand.inc(t1, -1);
    hand.inc(t2, -1);
    snap[lastDiscarder.index()].river.pop_back();
}

void Replay::lookDaiminkan(TableSnap &snap, TileCount &hand, Who who, Who lastDiscarder)
{
    T37 pick = snap[lastDiscarder.index()].river.back();

    std::array<T37, 3> pushes;
    pushes.fill(T37(pick.id34()));
    if (pick.val() == 5)
        for (int i = 0; i < 3; i++)
            if (i < hand.ct(pick.toAka5()))
                pushes[i] = pushes[i].toAka5();

    int lay = who.looksAt(lastDiscarder);
    int w = who.index();
    if (lay == 0)
        snap[w].barks.pushBack(M37::daiminkan(pick, pushes[0], pushes[1], pushes[2], lay));
    else if (lay == 1)
        snap[w].barks.pushBack(M37::daiminkan(pushes[0], pick, pushes[1], pushes[2], lay));
    else // lay == 2
        snap[w].barks.pushBack(M37::daiminkan(pushes[0], pushes[1], pick, pushes[2], lay));

    for (const T37 t : pushes)
        hand.inc(t, -1);

    snap[lastDiscarder.index()].river.pop_back();
}

void Replay::lookAnkan(TableSnap &snap, TileCount &hand, T34 t34, Who who)
{
    int w = who.index();
    if (hand.ct(t34) == 4) {
        std::array<T37, 4> pushes;
        pushes.fill(T37(t34.id34()));
        if (t34.val() == 5)
            for (int i = 0; i < 4; i++)
                if (i < hand.ct(pushes[i].toAka5()))
                    pushes[i] = pushes[i].toAka5();

        // four in hand
        snap[w].barks.pushBack(M37::ankan(pushes[0], pushes[1], pushes[2], pushes[3]));
        hand.inc(pushes[0], -1);
        hand.inc(pushes[1], -1);
        hand.inc(pushes[2], -1);
        hand.inc(pushes[3], -1);

        hand.inc(snap.drawn, 1);
        snap.whoDrawn = Who();
    } else {
        assert(hand.ct(t34) == 3 && snap.drawn == t34);
        assert(snap.whoDrawn == who);

        std::array<T37, 3> pushes;
        pushes.fill(T37(t34.id34()));
        if (t34.val() == 5)
            for (int i = 0; i < 3; i++)
                if (i < hand.ct(pushes[i].toAka5()))
                    pushes[i] = pushes[i].toAka5();

        snap[w].barks.pushBack(M37::ankan(pushes[0], pushes[1], pushes[2], snap.drawn));
        snap.whoDrawn = Who();
        hand.inc(pushes[0], -1);
        hand.inc(pushes[1], -1);
        hand.inc(pushes[2], -1);
    }
}

void Replay::lookKakan(TableSnap &snap, TileCount &hand, const T37 &t37, Who who)
{
    assert(snap.whoDrawn == who);

    if (snap.drawn != t37) { // from closed, not drawn
        hand.inc(t37, -1);
        hand.inc(snap.drawn, 1);
    }

    snap.whoDrawn = Who();

    auto &barks = snap[who.index()].barks;
    auto same = [&t37](const M37 &m) { return m[0] == t37; };
    auto it = std::find_if(barks.begin(), barks.end(), same);
    assert(it != barks.end());

    it->kakan(t37);
}



} // namespace saki
