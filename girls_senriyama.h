#ifndef SAKI_GIRLS_SENRIYAMA_H
#define SAKI_GIRLS_SENRIYAMA_H

#include "girl.h"
#include "table_observer.h"
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
    void onActivate(const Table &table, Choices &choices) override;
    void onInbox(Who who, const Action &action) override;
    Choices forwardAction(const Table &table, Mount &mount, const Action &action) override;

    std::string popUpStr() const override;

private:
    enum class PopUpMode { OO, FV };

    void popUpBy(const Table &table, PopUpMode mode);

private:
    Choices mCleanChoices;
    Choices mCrazyChoices;
    bool mInFuture = false;
    bool mCheckNextAction = false;
    std::vector<Action> mRecords;
    int mCd = 0;
    PopUpMode mPopUpMode;
    TokiEvents mEvents;
};



class Sera : public Girl
{
public:
    GIRL_CTORS(Sera)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



} // namespace saki



#endif // SAKI_GIRLS_SENRIYAMA_H


