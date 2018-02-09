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



#define BLANK_DEF { (void) t; (void) e; }

class TableObserverDispatched : public TableObserver
{
public:
    virtual ~TableObserverDispatched() = default;

    using TE = TableEvent;

    void onTableEvent(const Table &table, const TE &event) final
    {
        using T = TE::Type;
        switch (event.type()) {
        case T::TABLE_STARTED:
            onTableEvent(table, event.as<TE::TableStarted>());
            break;
        case T::FIRST_DEALER_CHOSEN:
            onTableEvent(table, event.as<TE::FirstDealerChosen>());
            break;
        case T::ROUND_STARTED:
            onTableEvent(table, event.as<TE::RoundStarted>());
            break;
        case T::CLEANED:
            onTableEvent(table, event.as<TE::Cleaned>());
            break;
        case T::DICED:
            onTableEvent(table, event.as<TE::Diced>());
            break;
        case T::DEALT:
            onTableEvent(table, event.as<TE::Dealt>());
            break;
        case T::FLIPPED:
            onTableEvent(table, event.as<TE::Flipped>());
            break;
        case T::DRAWN:
            onTableEvent(table, event.as<TE::Drawn>());
            break;
        case T::DISCARDED:
            onTableEvent(table, event.as<TE::Discarded>());
            break;
        case T::RIICHI_CALLED:
            onTableEvent(table, event.as<TE::RiichiCalled>());
            break;
        case T::RIICHI_ESTABLISHED:
            onTableEvent(table, event.as<TE::RiichiEstablished>());
            break;
        case T::BARKED:
            onTableEvent(table, event.as<TE::Barked>());
            break;
        case T::ROUND_ENDED:
            onTableEvent(table, event.as<TE::RoundEnded>());
            break;
        case T::POINTS_CHANGED:
            onTableEvent(table, event.as<TE::PointsChanged>());
            break;
        case T::TABLE_ENDED:
            onTableEvent(table, event.as<TE::TableEnded>());
            break;
        case T::POPPED_UP:
            onTableEvent(table, event.as<TE::PoppedUp>());
            break;
        }
    }

    virtual void onTableEvent(const Table &t, const TE::TableStarted &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::FirstDealerChosen &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::RoundStarted &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::Cleaned &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::Diced &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::Dealt &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::Flipped &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::Drawn &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::Discarded &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::RiichiCalled &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::RiichiEstablished &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::Barked &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::RoundEnded &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::PointsChanged &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::TableEnded &e) BLANK_DEF
    virtual void onTableEvent(const Table &t, const TE::PoppedUp &e) BLANK_DEF
};



} // namespace saki



#endif // SAKI_TABLE_OBSERVER_H
