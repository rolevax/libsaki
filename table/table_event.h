#ifndef SAKI_TABLE_EVENT_H
#define SAKI_TABLE_EVENT_H

#include "../form/form.h"
#include "../unit/action.h"
#include "../util/misc.h"

#include <cstdint>



namespace saki
{



enum class RoundResult
{
    // *** SYNC with string_enum.cpp ***
    TSUMO, RON, HP, KSKP, SFRT, SKSR, SCRC, SCHR, NGSMG, ABORT, NUM_ROUNDRES
};

class TableEvent
{
public:
    enum class Type
    {
        TABLE_STARTED, FIRST_DEALER_CHOSEN, ROUND_STARTED, CLEANED,
        DICED, DEALT, FLIPPED, DRAWN, DISCARDED,
        RIICHI_CALLED, RIICHI_ESTABLISHED, BARKED, ROUND_ENDED,
        POINTS_CHANGED, TABLE_ENDED, POPPED_UP
    };

    struct Args
    {
        virtual ~Args() = default;
        virtual std::unique_ptr<Args> clone() = 0;
    };

    template<typename ArgsT>
    struct ArgsCloneable : Args
    {
        explicit ArgsCloneable(const ArgsT &a) : args(a) {}

        std::unique_ptr<Args> clone() override
        {
            return util::unique<ArgsCloneable>(*this);
        }

        ArgsT args;
    };

    struct TableStarted
    {
        static const Type TYPE = Type::TABLE_STARTED;
        uint32_t seed;
    };

    struct FirstDealerChosen
    {
        static const Type TYPE = Type::FIRST_DEALER_CHOSEN;
        Who who;
    };

    struct RoundStarted
    {
        static const Type TYPE = Type::ROUND_STARTED;
        int round;
        int extraRound;
        Who dealer;
        bool allLast;
        int deposit;
        uint32_t seed;
    };

    struct Cleaned
    {
        static const Type TYPE = Type::CLEANED;
    };

    struct Diced
    {
        static const Type TYPE = Type::DICED;
        int die1;
        int die2;
    };

    struct Dealt
    {
        static const Type TYPE = Type::DEALT;
    };

    struct Flipped
    {
        static const Type TYPE = Type::FLIPPED;
    };

    struct Drawn
    {
        static const Type TYPE = Type::DRAWN;
        Who who;
    };

    struct Discarded
    {
        static const Type TYPE = Type::DISCARDED;
        bool spin;
    };

    struct RiichiCalled
    {
        static const Type TYPE = Type::RIICHI_CALLED;
        Who who;
    };

    struct RiichiEstablished
    {
        static const Type TYPE = Type::RIICHI_ESTABLISHED;
        Who who;
    };

    struct Barked
    {
        static const Type TYPE = Type::BARKED;
        Who who;
        M37 bark;
        bool spin;
    };

    struct RoundEnded
    {
        static const Type TYPE = Type::ROUND_ENDED;
        RoundResult result;
        util::Stactor<Who, 4> openers;
        Who gunner;
        std::vector<Form> forms; // FUCK change to stactor
    };

    struct PointsChanged
    {
        static const Type TYPE = Type::POINTS_CHANGED;
    };

    struct TableEnded
    {
        static const Type TYPE = Type::TABLE_ENDED;
        std::array<Who, 4> ranks;
        std::array<int, 4> scores;
    };

    struct PoppedUp
    {
        static const Type TYPE = Type::POPPED_UP;
        Who who;
    };

    template<typename ArgsT>
    TableEvent(ArgsT args)
        : mType(ArgsT::TYPE)
        , mArgs(util::unique<ArgsCloneable<ArgsT>>(args))
    {
    }

    ~TableEvent() = default;

    TableEvent(const TableEvent &copy)
        : mType(copy.mType)
        , mArgs(copy.mArgs->clone())
    {
    }

    Type type() const
    {
        return mType;
    }

    template<typename ArgsT>
    const ArgsT &as() const
    {
        assert(mType == ArgsT::TYPE);
        return (static_cast<const ArgsCloneable<ArgsT> &>(*mArgs)).args;
    }

private:
    Type mType;
    std::unique_ptr<Args> mArgs;
};



} // namespace saki



#endif // SAKI_TABLE_EVENT_H
