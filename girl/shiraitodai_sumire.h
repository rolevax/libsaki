#ifndef SAKI_GIRL_SHIRAITODAI_SUMIRE_H
#define SAKI_GIRL_SHIRAITODAI_SUMIRE_H

#include "../form/parsed_view.h"
#include "../table/irs_ctrl.h"



namespace saki
{



class Sumire : public Girl
{
public:
    GIRL_CTORS(Sumire)

    void onDice(util::Rand &rand, const Table &table) override;
    bool checkInit(Who who, const Hand &init, const Table &table, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onIrsChecked(const Table &table, Mount &mount) override;

    std::string popUpStr() const override;

    void onTableEvent(const Table &table, const TableEvent &event) override;

protected:
    IrsCtrlGetter attachIrsOnDrawn(const Table &table) override;

private:
    void handleDrawSelf(const Table &table, Mount &mount, bool rinshan);
    void handleDrawTarget(const Table &table, Mount &mount, bool rinshan);

    bool canStartAiming(const Table &table);
    bool canShootTarget(const Table &table);
    bool chooseFinalWait(const Table &table, Mount &mount, bool allowNum19, bool allowZ);
    int evalTargetHappy(const Table &table, const Mount &mount, const Hand &dream, const Action &act);
    void planAimming(const Table &table, Mount &mount);
    void updateFeedSelf(const util::Stactor<T34, 2> &plan);
    void updateFeedSelfByClamp(std::function<bool(T34)> missing);
    void updateFeedSelfByParse(ParsedView4Ready view, Mount &mount);

    void shapeYaku(const Table &table, Mount &mount, bool rinshan);
    bool shapeYakuhai(const Table &table, Mount &mount, bool rinshan);
    bool shapeTanyao(const Table &table, Mount &mount, bool rinshan);

private:
    // *** SYNC order with 'mIrsCtrl' ***
    enum IrsIndex { MAIN, RIGHT, CROSS, LEFT, ALLOW_NUM19, ALLOW_Z };

    IrsCtrlClickCheck<Sumire> mIrsCtrl {
        Choices::ModeIrsCheck {
            "SUMIRE",
            IrsCheckList {
                IrsCheckItem::CHECK_ENABLED,
                IrsCheckItem::CHILD_RADIO_DEFAULT,
                IrsCheckItem::CHILD_RADIO,
                IrsCheckItem::CHILD_RADIO,
                IrsCheckItem::CHILD_CHECK,
                IrsCheckItem::CHILD_CHECK
            }
        },
        &Sumire::mIrsCtrl
    };

    Who mTarget;
    T34 mFinalWait;
    T37 mFeedTarget;
    util::Stactor<T34, 2> mFeedSelf;
    std::bitset<34> mTargetShits;
    int mShootTrial = 0;
};



} // namespace saki



#endif // SAKI_GIRL_SHIRAITODAI_SUMIRE_H
