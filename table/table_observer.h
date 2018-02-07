#ifndef SAKI_TABLE_OBSERVER_H
#define SAKI_TABLE_OBSERVER_H

#include "table_event.h"

#include <vector>
#include <array>



namespace saki
{



// 24 is the theoritical maximum
using River = util::Stactor<T37, 24>;

class Table;



class TableObserver
{
public:
    virtual ~TableObserver() = default;

    virtual void onTableEvent(const Table &table, const TableEvent &event)
    {
        (void) table; (void) event;
    }
};



class TableObserverDispatched : public TableObserver
{
public:
    virtual ~TableObserverDispatched() = default;

    void onTableEvent(const Table &table, const TableEvent &event) final
    {
        using T = TableEvent::Type;
        switch (event.type()) {
        case T::TABLE_STARTED:
            // onTableStarted(table, event.as<TableEvent::TableStarted>());
            break;
        case T::FIRST_DEALER_CHOSEN:
            break;
        case T::ROUND_STARTED:
            break;
        case T::CLEANED:
            break;
        case T::DICED:
            break;
        case T::DEALT:
            break;
        case T::FLIPPED:
            break;
        case T::DRAWN:
            break;
        case T::DISCARDED:
            break;
        case T::RIICHI_CALLED:
            break;
        case T::RIICHI_ESTABLISHED:
            break;
        case T::BARKED:
            break;
        case T::ROUND_ENDED:
            break;
        case T::POINTS_CHANGED:
            break;
        case T::TABLE_ENDED:
            break;
        case T::POPPED_UP:
            break;
        }
    }

    // virtual void onTableStarted(const Table &table, const TableEvent::TableStarted &event) = 0;

    // deprecated
    virtual void onTableStarted(const Table &table, uint32_t seed)
    {
        (void) seed;
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

    virtual void onRiichiEstablished(const Table &table, Who who)
    {
        (void) table; (void) who;
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



#endif // SAKI_TABLE_OBSERVER_H
