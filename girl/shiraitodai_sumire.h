#ifndef SAKI_GIRL_SHIRAITODAI_SUMIRE_H
#define SAKI_GIRL_SHIRAITODAI_SUMIRE_H

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

    bool aimable(const Table &table);
    bool shootable(const Table &table);
    void pickBullet(const Table &table, Mount &mount);
    void planAimming(const Table &table);
    void updateFeedSelf(util::Stactor<T34, 2> plan);
    void updateFeedSelfByClamp(std::function<bool(T34)> missing);

    void shapeYaku(const Table &table, Mount &mount, bool rinshan);
    bool shapeYakuhai(const Table &table, Mount &mount, bool rinshan);
    bool shapeTanyao(const Table &table, Mount &mount, bool rinshan);

private:
    // *** SYNC order with 'mIrsCtrl' ***
    enum IrsIndex { MAIN, RIGHT, CROSS, LEFT };

    IrsCtrlClickCheck<Sumire> mIrsCtrl {
        Choices::ModeIrsCheck {
            "SUMIRE",
            IrsCheckList {
                IrsCheckItem::CHECK_ENABLED,
                IrsCheckItem::CHILD_RADIO_DEFAULT,
                IrsCheckItem::CHILD_RADIO,
                IrsCheckItem::CHILD_RADIO,
            }
        },
        &Sumire::mIrsCtrl
    };

    Who mTarget;
    T34 mWant;
    T37 mFeed;
    util::Stactor<T34, 2> mFeedSelf;
    int mShootTrial;
};



} // namespace saki



#endif // SAKI_GIRL_SHIRAITODAI_SUMIRE_H
