#ifndef SAKI_GIRLS_SENRIYAMA_H
#define SAKI_GIRLS_SENRIYAMA_H

#include "../table/irs_ctrl.h"
#include "girls_util_toki.h"
#include "../table/table_observer.h"

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
    void onInbox(Who who, const Action &action) override;

    std::string popUpStr() const override;

protected:
    IrsCtrlGetter attachIrsOnDrawn(const Table &table) override;

private:
    class PredictCtrl : public IrsCtrlPlus<Toki>
    {
    public:
        const Choices &choices() const override;
        IrsResult handle(Toki &toki, const Table &table, Mount &mount, const Action &action) override;
        void setClickHost(Choices normal);

    private:
        Choices mChoices;
    };

    enum class PopUpMode { OO, FV };

    static std::array<Girl::Id, 4> makeIdArray(const Table &table);

    IrsResult handleIrs(const Table &table, Mount &mount, const Action &action);
    IrsResult handleIrsClick();
    IrsResult handlePredict(const Table &table, Mount &mount, const Action &action);
    void popUpBy(const Table &table, PopUpMode mode);

private:
    PredictCtrl mIrsCtrl;
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
