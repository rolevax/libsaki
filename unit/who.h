#ifndef SAKI_WHO_H
#define SAKI_WHO_H

#include "../util/assume.h"
#include "../util/int_iter.h"

#define valid_somebody(w) (0 <= w && w < 4)



namespace saki
{



class Who
{
public:
    static constexpr int HUMAN = 0;
    static constexpr IntRange<Who, 4> ALL4 = IntRange<Who, 4>();

    Who() : mWho(NOBODY) {}

    explicit Who(int who)
        : mWho(who)
    {
        assert(valid_somebody(mWho));
    }

    explicit Who(unsigned who)
        : mWho(static_cast<decltype(mWho)>(who))
    {
        assert(0 <= mWho && mWho <= NOBODY);
    }

    Who(const Who &copy) = default;

    int index() const
    {
        assert(valid_somebody(mWho));
        assume_opt_out(valid_somebody(mWho));
        return mWho;
    }

    Who right() const
    {
        assert(valid_somebody(mWho));
        assume_opt_out(valid_somebody(mWho));
        return Who((mWho + 1) % 4);
    }

    Who left() const
    {
        assert(valid_somebody(mWho));
        assume_opt_out(valid_somebody(mWho));
        return Who((mWho + 3) % 4);
    }

    Who cross() const
    {
        assert(valid_somebody(mWho));
        assume_opt_out(valid_somebody(mWho));
        return Who((mWho + 2) % 4);
    }

    Who byDice(int dice) const
    {
        assert(1 <= dice && dice <= 12);
        assume_opt_out(1 <= dice && dice <= 12);
        return Who((mWho + dice - 1) % 4);
    }

    Who byTurn(int turn) const
    {
        return Who((mWho + turn) % 4);
    }

    ///
    /// \brief Relative position of anohter player
    /// \param tar The other player
    /// \return 0 if tar is on this's left, 1 if cross, 2 if right.
    ///
    int looksAt(Who tar) const
    {
        assert(valid_somebody(mWho));
        assume_opt_out(valid_somebody(mWho));
        assert(valid_somebody(tar.mWho));
        assume_opt_out(valid_somebody(tar.mWho));
        assert(mWho != tar.mWho);
        assume_opt_out(mWho != tar.mWho);
        return 3 - (tar.mWho - mWho + 4) % 4;
    }

    ///
    /// \brief Turning count from another players
    /// \param from The other player
    /// \return 0 if self, 1 if this is from's right, 2 if cross, 3 if left
    ///
    int turnFrom(Who from) const
    {
        assert(valid_somebody(mWho));
        assume_opt_out(valid_somebody(mWho));
        assert(valid_somebody(from.mWho));
        assume_opt_out(valid_somebody(from.mWho));
        return (4 + mWho - from.mWho) % 4;
    }

    bool somebody() const
    {
        return mWho != NOBODY;
    }

    bool nobody() const
    {
        return mWho == NOBODY;
    }

    bool human() const
    {
        return mWho == HUMAN;
    }

    bool operator==(Who rhs) const
    {
        return mWho == rhs.mWho;
    }

    bool operator!=(Who rhs) const
    {
        return !(*this == rhs);
    }

    Who &operator=(const Who &assign) = default;

private:
    static const int NOBODY = 4;

    int mWho;
};



} // namespace saki



#endif // SAKI_WHO_H
