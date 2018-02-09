#ifndef SAKI_REPLAY_H
#define SAKI_REPLAY_H

#include "../table/table_observer.h"
#include "../table/girl.h"



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
    util::Stactor<T37, 5> drids;
    util::Stactor<T37, 5> urids;
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

class Replay : public TableObserverDispatched
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
        explicit InAct(In act) : act(act) {}
        explicit InAct(In act, const T37 &t) : act(act), t37(t) {}
        explicit InAct(In act, int showAka5) : act(act), showAka5(showAka5) {}
        In act;
        union
        {
            T37 t37;
            int showAka5;
        };

    };

    struct OutAct
    {
        explicit OutAct(Out act) : act(act) {}
        explicit OutAct(Out act, const T37 &t) : act(act), t37(t) {}
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

    void onTableEvent(const Table &table, const TE::TableStarted &event) override;
    void onTableEvent(const Table &table, const TE::RoundStarted &event) override;
    void onTableEvent(const Table &table, const TE::Diced &event) override;
    void onTableEvent(const Table &table, const TE::Dealt &event) override;
    void onTableEvent(const Table &table, const TE::Flipped &event) override;
    void onTableEvent(const Table &table, const TE::Drawn &event) override;
    void onTableEvent(const Table &table, const TE::Discarded &event) override;
    void onTableEvent(const Table &table, const TE::RiichiCalled &event) override;
    void onTableEvent(const Table &table, const TE::Barked &event) override;
    void onTableEvent(const Table &table, const TE::RoundEnded &event) override;
    void onTableEvent(const Table &table, const TE::PointsChanged &event) override;

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
    Rule rule;
    uint32_t seed;
    std::vector<Round> rounds;

private:
    bool mToEstablishRiichi = false;
};



} // namespace saki



#endif // SAKI_REPLAY_H
