#ifndef GIRLS_SENRIYAMA_H
#define GIRLS_SENRIYAMA_H

#include "girl.h"
#include "tableobserver.h"
#include "ticketfolder.h"
#include "girls_util_toki.h"

#include <sstream>



namespace saki
{



class TokiEvent;

class Toki : public Girl
{
public:
    GIRL_CTORS(Toki)

    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onActivate(const Table &table, TicketFolder &tickets) override;
    void onInbox(Who who, const Action &action) override;
    TicketFolder forwardAction(const Table &table, Mount &mount, const Action &action) override;

    std::string popUpStr() const override;

private:
    enum class PopUpMode { GREEN, OO, FV };

    void popUpBy(const Table &table, PopUpMode mode);

private:
    TicketFolder mCleanTickets;
    TicketFolder mCrazyTickets;
    bool mInFuture = false;
    bool mToRiichi = false; // resuing in forwarding and inbox
    bool mCheckNextAction = false;
    std::vector<std::pair<ActCode, int>> mRecords;
    int mCd = 0;
    PopUpMode mPopUpMode;
    TokiEvents mEvents;
};



} // namespace saki



#endif // GIRLS_SENRIYAMA_H


