#include "ticketfolder.h"
#include "util.h"

#include <numeric>



namespace saki
{



TicketFolder::TicketFolder()
{
    disableAll();
}

TicketFolder::TicketFolder(ActCode only)
{
    disableAll();
    enable(only);
}

Action TicketFolder::sweep() const
{
    if (mForwardAll && can(ActCode::IRS_CLICK))
        return Action(IRS_CLICK); // toki: exit future

    std::vector<ActCode> just {
        ActCode::NEXT_ROUND, ActCode::END_TABLE, ActCode::DICE,
        ActCode::SPIN_OUT, ActCode::PASS
    };

    for (ActCode act : just)
        if (can(act))
            return Action(act);

    if (can(ActCode::SWAP_OUT) && !mSwappables.empty())
        return Action(ActCode::SWAP_OUT, mSwappables.back());

    if (can(ActCode::IRS_CHECK)) // assume 0u is always legal
        return Action(ActCode::IRS_CHECK, 0u);

    if (can(ActCode::IRS_RIVAL)) {
        return Action(ActCode::IRS_RIVAL, Who());
    }

    return Action(ActCode::NOTHING);
}

bool TicketFolder::forwardAll() const
{
    return mForwardAll;
}

bool TicketFolder::forwardAny() const
{
    return forwardAll()
            || can(ActCode::IRS_CHECK)
            || can(ActCode::IRS_CLICK)
            || can(ActCode::IRS_RIVAL);
}

bool TicketFolder::any() const
{
    return util::has(mTickets, true);
}

bool TicketFolder::can(ActCode act) const
{
    return mTickets[act];
}

bool TicketFolder::spinOnly() const
{
    int sum = std::accumulate(mTickets.begin(), mTickets.end(), 0);
    return mTickets[ActCode::SPIN_OUT] && sum == 1;
}

const std::vector<T37> &TicketFolder::swappables() const
{
    return mSwappables;
}

const std::vector<T34> &TicketFolder::ankanables() const
{
    return mAnkanables;
}

const std::vector<int> &TicketFolder::kakanables() const
{
    return mKakanables;
}

void TicketFolder::setForwarding(bool v)
{
    mForwardAll = v;
}

void TicketFolder::enable(ActCode act)
{
    mTickets[act] = true;
}

void TicketFolder::enableSwapOut(const std::vector<T37> &choices)
{
    assert(!choices.empty());
    mTickets[ActCode::SWAP_OUT] = true;
    mSwappables = choices;
}

void TicketFolder::enableAnkan(const std::vector<T34> &choices)
{
    assert(!choices.empty());
    mTickets[ActCode::ANKAN] = true;
    mAnkanables = choices;
}

void TicketFolder::enableKakan(const std::vector<int> &choices)
{
    assert(!choices.empty());
    mTickets[ActCode::KAKAN] = true;
    mKakanables = choices;
}

void TicketFolder::disableAll()
{
    mTickets.fill(false);
}



} // namespace saki


