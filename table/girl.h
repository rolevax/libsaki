#ifndef SAKI_GIRL_H
#define SAKI_GIRL_H

#include "../table/mount.h"
#include "../table/table_observer.h"
#include "../table/choices.h"

#include "../util/misc.h"

#include <bitset>
#include <optional>



#define GIRL_CTORS(Name) \
    Name(Who who, Id id) : Girl(who, id) {} \
    Name(const Name &copy) = default; \
    std::unique_ptr<Girl> clone() const override \
    { \
        return std::make_unique<Name>(*this); \
    }

#define GIRL_COPY_CTORS(Name) \
    Name(const Name &copy) = default; \
    std::unique_ptr<Girl> clone() const override \
    { \
        return std::make_unique<Name>(*this); \
    }



namespace saki
{



class Hand;
class Table;
class Choices;
class Action;
class IrsCtrl;
class HrhInitFix;
class HrhBargainer;



///
/// \brief Base class for skill implementation
///
/// Girl's TableObserver callbacks will be called before ordinary observers.
/// It is not needed to add girls to the observer vector when constructing a Table.
///
class Girl : public TableObserver
{
public:
    // *INDENT-OFF*
    enum class Id
    {
        DOGE = 0, CUSTOM = 1,

        // IH71 A-Block
        MIYANAGA_TERU = 710111, HIROSE_SUMIRE = 710112, SHIBUYA_TAKAMI = 710113,
            MATANO_SEIKO = 710114, OOHOSHI_AWAI = 710115,
        MATSUMI_KURO = 712411, MATSUMI_YUU = 712412, ATARASHI_AKO = 712413,
            SAGIMORI_ARATA = 712414, TAKAKAMO_SHIZUNO = 712415,
        ONJOUJI_TOKI = 712611, NIJOU_IZUMI = 712612, EGUCHI_SERA = 712613,
            FUNAKUBO_HIROKO = 712614, SHIMIZUDANI_RYUUKA = 712615,

        // IH71 B-Block
        USUZUMI_HATSUMI = 712714, IWATO_KASUMI = 712715,
        ANETAI_TOYONE = 712915,
        KATAOKA_YUUKI = 713311, SOMEYA_MAKO = 713312, TAKEI_HISA = 713313,
            HARAMURA_NODOKA = 713314, MIYANAGA_SAKI = 713315,
            NANPO_KAZUE = 713301,
        UESHIGE_SUZU = 713811, SUEHARA_KYOUKO = 713815,
        TSUJIGAITO_SATOHA = 715211, HAO_HUIYU = 715212, CHOI_MYEONGHWA = 715213,
            MEGAN_DAVIN = 715214, NELLY_VIRSALADZE = 715215,
        SHISHIHARA_SAWAYA = 714915,

        // Others
        INAMURA_KYOUKA = 990001, SHIRATSUKI_SHINO = 990002, HONDOU_YUE = 990003,
            MIHOROGI_UTA = 990011, TAKUWA_RIO = 990014,
        KAJINO_YUI = 990024
    };
    // *INDENT-ON*

    ///
    /// \brief Wrapper of a "T Girl::*" value where T implements IrsCtrl
    ///
    /// benefits:
    /// - independent from Girl instance, safe to copy
    /// - transparent to the dynamic type of IrsCtrl implementation
    /// - clean syntax
    ///
    class IrsCtrlGetter
    {
    public:
        template<typename IrsCtrlImpl, typename GirlImpl>
        static IrsCtrlImpl Girl::*upcast(IrsCtrlImpl GirlImpl::*mem)
        {
            return static_cast<IrsCtrlImpl Girl::*>(mem);
        }

        template<typename IrsCtrlImpl, typename GirlImpl>
        IrsCtrlGetter(IrsCtrlImpl GirlImpl::*mem)
            : mImpl(mem == nullptr ? nullptr : std::make_unique<Impl<IrsCtrlImpl>>(upcast(mem)))
        {
        }

        IrsCtrlGetter(std::nullptr_t)
        {
        }

        IrsCtrlGetter(const IrsCtrlGetter &copy)
            : mImpl(copy.mImpl == nullptr ? nullptr : copy.mImpl->clone())
        {
        }

        IrsCtrlGetter(IrsCtrlGetter &&move)
            : mImpl(std::move(move.mImpl))
        {
        }

        IrsCtrlGetter &operator=(IrsCtrlGetter &&that)
        {
            mImpl = std::move(that.mImpl);
            return *this;
        }

        bool ready() const
        {
            return mImpl != nullptr;
        }

        IrsCtrl &get(Girl &girl) const
        {
            return mImpl->get(girl);
        }

    private:
        class ImplBase
        {
        public:
            virtual ~ImplBase() = default;
            virtual std::unique_ptr<ImplBase> clone() = 0;
            virtual IrsCtrl &get(Girl &girl) = 0;
        };

        template<typename IrsCtrlType>
        class Impl : public ImplBase
        {
        public:
            Impl(IrsCtrlType Girl::*ptrToMem)
                : mPtrToMem(ptrToMem)
            {
            }

            std::unique_ptr<ImplBase> clone() override
            {
                return std::make_unique<Impl>(*this);
            }

            IrsCtrl &get(Girl &girl) override
            {
                return girl.*mPtrToMem;
            }

        private:
            IrsCtrlType Girl::*mPtrToMem;
        };

    private:
        std::unique_ptr<ImplBase> mImpl;
    };

    static std::unique_ptr<Girl> create(Who who, int id);

    Girl(Who who, Id id);
    virtual ~Girl() = default;
    Girl(const Girl &copy) = default;
    Girl &operator=(const Girl &assign) = delete;

    virtual std::unique_ptr<Girl> clone() const;

    Id getId() const;

    virtual void onDice(util::Rand &rand, const Table &table);
    virtual void onMonkey(std::array<Exist, 4> &exists, const Table &table);
    virtual bool checkInit(Who who, const Hand &init, const Table &table, int iter);
    virtual void onInbox(Who who, const Action &action);
    virtual void onDraw(const Table &table, Mount &mount, Who who, bool rinshan);
    virtual void onChooseFirstDealer(util::Rand &rand, Who tempDealer, int &die1, int &die2);
    virtual void onIrsChecked(const Table &table, Mount &mount);

    void onFilterChoice(const Table &table, Who who, ChoiceFilter &filter);
    void onActivateDice(const Table &table);
    void onActivate(const Table &table);

    bool irsReady() const;
    const Choices &irsChoices() const;

    virtual std::optional<HrhInitFix> onHrhRaid(const Table &table);
    virtual HrhBargainer *onHrhBargain(const Table &table);
    virtual std::optional<HrhInitFix> onHrhBeg(util::Rand &rand, const TileCount &stock);

    virtual std::string popUpStr() const;

    bool handleIrs(const Table &table, Mount &mount, const Action &action);

protected:
    static void eraseRivered(util::Stactor<T34, 34> &ts, const River &river);
    static void eraseRivered(std::bitset<34> &ts, const River &river);
    void accelerate(Mount &mount, const Hand &hand, const River &river, int delta);

    virtual ChoiceFilter filterChoice(const Table &table, Who who);
    virtual IrsCtrlGetter attachIrsOnDice();
    virtual IrsCtrlGetter attachIrsOnDrawn(const Table &table);

protected:
    const Who mSelf;
    const Id mId;

private:
    IrsCtrlGetter mIrsCtrlGetter = nullptr;
};



} // namespace saki



#endif // SAKI_GIRL_H
