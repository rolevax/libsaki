#ifndef SAKI_TICKETFOLDER_H
#define SAKI_TICKETFOLDER_H

#include "action.h"



namespace saki
{



class TicketFolder
{
public:
    TicketFolder();
    explicit TicketFolder(ActCode only);

    TicketFolder(const TicketFolder &copy) = default;
    TicketFolder &operator=(const TicketFolder& assign) = default;

    Action sweep() const;

    bool forwardAll() const;
    bool forwardAny() const;
    bool any() const;
    bool can(ActCode act) const;
    bool can(const Action &act) const;
    bool spinOnly() const;
    bool swappable(T34 tile) const;
    const std::vector<T37> &swappables() const;
    const std::vector<T34> &ankanables() const;
    const std::vector<int> &kakanables() const;

    void setForwarding(bool v);
    void enable(ActCode act);
    void enableSwapOut(const std::vector<T37> &choices);
    void enableAnkan(const std::vector<T34> &choices);
    void enableKakan(const std::vector<int> &choices);
    void disableAll();

private:
    std::array<bool, ActCode::NUM_ACTCODE> mTickets;
    bool mForwardAll = false;

    // vectors are preferred for ease of iteration
    std::vector<T37> mSwappables;
    std::vector<int> mKakanables;
    std::vector<T34> mAnkanables;
};



} // namespace saki



#endif // SAKI_TICKETFOLDER_H


