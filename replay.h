#ifndef SAKI_REPLAY_H
#define SAKI_REPLAY_H

#include "table_observer.h"
#include "girl.h"



namespace saki
{



struct PlayerSnap
{
    util::Stactor<T37, 13> hand;
    util::Stactor<M37, 4> barks;
    std::vector<T37> river;
    int riichiPos = -1;
    bool riichiBar = false;
};

struct TableSnap
{
    std::array<PlayerSnap, 4> players;
    Who whoDrawn;
    T37 drawn;
    std::array<int, 4> points;
    std::vector<T37> drids;
    std::vector<T37> urids;
    int wallRemain;
    int deadRemain;

    int round;
    int extraRound;
    Who dealer;
    bool allLast;
    int deposit;
    uint32_t state;
    int die1;
    int die2;
    RoundResult result;
    bool endOfRound = false;
    Who gunner;
    T37 cannon;
    std::vector<Who> openers;
    std::vector<std::string> spells;
    std::vector<std::string> charges;

    PlayerSnap &operator[](int w) { return players[w]; }
    const PlayerSnap &operator[](int w) const { return players[w]; }
};

class Replay : public TableObserver
{
public:
    enum In
    {
        DRAW, CHII_AS_LEFT, CHII_AS_MIDDLE, CHII_AS_RIGHT,
        PON, DAIMINKAN, RON, SKIP_IN
    };

    enum Out
    {
        ADVANCE, SPIN, RIICHI_ADVANCE, RIICHI_SPIN, ANKAN, KAKAN,
        RYUUKYOKU, TSUMO, SKIP_OUT
    };

    struct InAct
    {
        explicit InAct(In act) : act(act) { }
        explicit InAct(In act, const T37 &t) : act(act), t37(t) { }
        explicit InAct(In act, int showAka5) : act(act), showAka5(showAka5) { }
        In act;
        union
        {
            T37 t37;
            int showAka5;
        };
    };

    struct OutAct
    {
        explicit OutAct(Out act) : act(act) { }
        explicit OutAct(Out act, const T37 &t) : act(act), t37(t) { }
        Out act;
        T37 t37;
    };

    struct Track
    {
        std::array<T37, 13> init;
        std::vector<InAct> in;
        std::vector<OutAct> out;
    };

    struct Round
    {
        int round;
        int extraRound;
        Who dealer;
        bool allLast;
        int deposit;
        uint32_t state;
        int die1;
        int die2;
        RoundResult result = RoundResult::ABORT;
        std::array<int, 4> resultPoints;
        std::vector<std::string> spells;
        std::vector<std::string> charges;
        std::vector<T37> drids;
        std::vector<T37> urids;
        std::array<Track, 4> tracks;
    };

    Replay() = default;
    Replay(const Replay &copy) = default;
    ~Replay() = default;
    Replay &operator=(const Replay &assign) = default;

    void onTableStarted(const Table &table, uint32_t seed) override;
    void onRoundStarted(int round, int extra, Who dealer,
                        bool al, int deposit, uint32_t seed) override;
    void onDiced(const Table &table, int die1, int die2) override;
    void onDealt(const Table &table) override;
    void onFlipped(const Table &table) override;
    void onDrawn(const Table &table, Who who) override;
    void onDiscarded(const Table &table, bool spin) override;
    void onRiichiCalled(Who who) override;
    void onBarked(const Table &table, Who who, const M37 &bark, bool spin) override;
    void onRoundEnded(const Table &table, RoundResult result,
                      const std::vector<Who> &openers, Who gunner,
                      const std::vector<Form> &fs) override;
    void onPointsChanged(const Table &table) override;

    TableSnap look(int roundId, int turn);

private:
    void addSkip(Who who, Who fromWhom);
    void lookAdvance(TableSnap &snap, TileCount &hand, const T37 &t37, Who who);
    void lookChii(TableSnap &snap, TileCount &hand, const InAct &in, Who who, Who lastDiscarder);
    void lookPon(TableSnap &snap, TileCount &hand, int showAka5, Who who, Who lastDiscarder);
    void lookDaiminkan(TableSnap &snap, TileCount &hand, Who who, Who lastDiscarder);
    void lookAnkan(TableSnap &snap, TileCount &hand, T34 t34, Who who);
    void lookKakan(TableSnap &snap, TileCount &hand, const T37 &t37, Who who);

public:
    std::array<Girl::Id, 4> girls;
    std::array<int, 4> initPoints;
    RuleInfo rule;
    uint32_t seed;
    std::vector<Round> rounds;

private:
    bool mToEstablishRiichi = false;
};



} // namespace saki



#endif // SAKI_REPLAY_H


