#ifndef SAKIIRS_CHANCE_H
#define SAKIIRS_CHANCE_H

#include "table.h"

#include <type_traits>



namespace saki
{



/// In-Round Skill handling result
struct IrsResult
{
    bool handled;
    Girl::IrsCtrlGetter next; // getter of next controller in an IRS-Chain
};



/// In-Round Skill Controller
class IrsCtrl
{
public:
    virtual ~IrsCtrl() = default;
    virtual const Choices &choices() const = 0;
    virtual IrsResult handle(Girl &girl, const Table &table, Mount &mount, const Action &action) = 0;
};



/// In-Round Skill Controller, improved
template<typename G>
class IrsCtrlPlus : public IrsCtrl
{
public:
    virtual IrsResult handle(G &girl, const Table &table, Mount &mount, const Action &action) = 0;

    IrsResult handle(Girl &girl, const Table &table, Mount &mount, const Action &action) final
    {
        return handle(static_cast<G &>(girl), table, mount, action);
    }
};



/// Controller of simple IRS-Check (typically before dice)
template<typename G>
class IrsCtrlCheck : public IrsCtrlPlus<G>
{
public:
    IrsCtrlCheck(Choices::ModeIrsCheck mode)
    {
        mChoices.setIrsCheck(mode);
    }

    const Choices &choices() const override
    {
        return mChoices;
    }

    IrsResult handle(G &girl, const Table &table, Mount &mount, const Action &action) override
    {
        setMode(action.mask());
        girl.onIrsChecked(table, mount);
        return { true, nullptr };
    }

    const IrsCheckItem &itemAt(int index) const
    {
        return mChoices.irsCheck().list.at(index);
    }

    void setAbleAt(int index, bool v)
    {
        Choices::ModeIrsCheck mode = mChoices.irsCheck();
        mode.list[index].setAble(v);
        mChoices.setIrsCheck(mode);
    }

    void setOnAt(int index, bool v)
    {
        Choices::ModeIrsCheck mode = mChoices.irsCheck();
        mode.list[index].setOn(v);
        mChoices.setIrsCheck(mode);
    }

private:
    void setMode(unsigned mask)
    {
        Choices::ModeIrsCheck mode = mChoices.irsCheck();
        for (size_t i = 0; i < mode.list.size(); i++) {
            bool on = mask & (1 << (mode.list.size() - 1 - i));
            mode.list[i].setOn(on);
        }

        mChoices.setIrsCheck(mode);
    }

private:
    Choices mChoices;
};



/// Controller of simple IRS-Click-and-Check (typically after drawn)
template<typename G>
class IrsCtrlClickCheck : public IrsCtrlCheck<G>
{
public:
    IrsCtrlClickCheck(Choices::ModeIrsCheck mode, IrsCtrlClickCheck G::*mem)
        : IrsCtrlCheck<G>(mode)
        , mPtrToMem(mem)
    {
    }

    const Choices &choices() const override
    {
        return mCheck ? IrsCtrlCheck<G>::choices() : mChoicesClick;
    }

    IrsResult handle(G &girl, const Table &table, Mount &mount, const Action &action) override
    {
        switch (action.act()) {
        case ActCode::IRS_CLICK:
            mCheck = true;
            return { true, mPtrToMem };
        case ActCode::IRS_CHECK:
            mCheck = false;
            return IrsCtrlCheck<G>::handle(girl, table, mount, action);
        default:
            return { false, nullptr };
        }
    }

    void setClickHost(Choices normal)
    {
        mChoicesClick = normal;
        mChoicesClick.setExtra(true);
    }

private:
    Choices mChoicesClick;
    bool mCheck = false;
    IrsCtrlClickCheck G::*mPtrToMem;
};



} // namespace saki



#endif // SAKIIRS_CHANCE_H
