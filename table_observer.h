#ifndef SAKI_TABLEOBSERVER_H
#define SAKI_TABLEOBSERVER_H

#include "action.h"
#include "tile.h"
#include "form.h"

#include <cstdint>
#include <vector>
#include <array>



namespace saki
{



class Table;
class Player;

enum class RoundResult
{
    // *** SYNC with string_enum.cpp ***
    TSUMO, RON, HP, KSKP, SFRT, SKSR, SCRC, SCHR, NGSMG, ABORT, NUM_ROUNDRES
};

class TableObserver
{
public:
    TableObserver() = default;
    TableObserver(const TableObserver &copy) = default;
    TableObserver &operator=(const TableObserver &assign) = default;
    virtual ~TableObserver() = default;

    virtual void onTableStarted(const Table &table, uint32_t seed)
    {
        (void) table; (void) seed;
    }

    virtual void onFirstDealerChoosen(Who initDealer)
    {
        (void) initDealer;
    }

    virtual void onRoundStarted(int round, int extra, Who dealer,
                                bool al, int deposit, uint32_t seed)
    {
        (void) round; (void) extra; (void) dealer;
        (void) al; (void) deposit; (void) seed;
    }

    virtual void onCleaned()
    {

    }

    virtual void onDiced(const Table &table, int die1, int die2)
    {
        (void) table; (void) die1; (void) die2;
    }

    virtual void onDealt(const Table &table)
    {
        (void) table;
    }

    virtual void onFlipped(const Table &table)
    {
        (void) table;
    }

    virtual void onDrawn(const Table &table, Who who)
    {
        (void) table; (void) who;
    }

    virtual void onDiscarded(const Table &table, bool spin)
    {
        (void) table; (void) spin;
    }

    virtual void onRiichiCalled(Who who)
    {
        (void) who;
    }

    virtual void onRiichiEstablished(Who who)
    {
        (void) who;
    }

    virtual void onBarked(const Table &table, Who who, const M37 &bark, bool spin)
    {
        // M37 param is required to know if kakan or not
        (void) table; (void) who; (void) bark; (void) spin;
    }

    virtual void onRoundEnded(const Table &table, RoundResult result,
                              const std::vector<Who> &openers, Who gunner,
                              const std::vector<Form> &fs)
    {
        (void) table; (void) result; (void) openers; (void) gunner; (void) fs;
    }

    virtual void onPointsChanged(const Table &table)
    {
        (void) table;
    }

    virtual void onTableEnded(const std::array<Who, 4> &rank,
                              const std::array<int, 4> &scores)
    {
        (void) rank; (void) scores;
    }

    virtual void onPoppedUp(const Table &table, Who who)
    {
        (void) table; (void) who;
    }
};



} // namespace saki



#endif // SAKI_TABLEOBSERVER_H


