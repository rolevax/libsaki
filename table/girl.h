#ifndef SAKI_GIRL_H
#define SAKI_GIRL_H

#include "../table/mount.h"
#include "../table/table_observer.h"
#include "../table/choices.h"

#include <bitset>



#define GIRL_CTORS(Name) \
    Name(Who who, Id id) : Girl(who, id) {} \
    Name(const Name &copy) = default; \
    Name *clone() const override { return new Name(*this); }



namespace saki
{



class Princess;
class Hand;
class Table;
class Choices;
class Action;
class IrsCtrl;



class Girl
{
public:
    // *INDENT-OFF*
    enum class Id
    {
        DOGE = 0,

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

    enum NmSkill
    {
        WHITE_CLOUD, ZIM_M, ZIM_P, ZIM_S,
        NUM_NM_SKILL
    };

    /// \brief Wrapper of a "T Girl::*" value where T implements IrsCtrl
    /// benefits:
    /// - independent from Girl instance, safe to copy
    /// - transparent to the dynamic type of IrsCtrl implementation
    /// - clean syntax
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
            : mImpl(mem == nullptr ? nullptr : new Impl<IrsCtrlImpl>(upcast(mem)))
        {
        }

        IrsCtrlGetter(std::nullptr_t)
        {
        }

        IrsCtrlGetter(const IrsCtrlGetter &copy)
            : mImpl(copy.mImpl == nullptr ? nullptr : copy.mImpl->clone())
        {
        }

        IrsCtrlGetter &operator=(const IrsCtrlGetter &that)
        {
            mImpl.reset(that.mImpl == nullptr ? nullptr : that.mImpl->clone());
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
            virtual ImplBase *clone() = 0;
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

            ImplBase *clone() override
            {
                return new Impl(*this);
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

    static Girl *create(Who who, int id);
    virtual Girl *clone() const;
    virtual ~Girl() = default;
    Girl &operator=(const Girl &assign) = delete;

    Id getId() const;

    virtual void onDice(util::Rand &rand, const Table &table);
    virtual void onMonkey(std::array<Exist, 4> &exists, const Princess &princess);
    virtual bool checkInit(Who who, const Hand &init, const Princess &princess, int iter);
    virtual void onInbox(Who who, const Action &action);
    virtual void onDraw(const Table &table, Mount &mount, Who who, bool rinshan);
    virtual void onChooseFirstDealer(util::Rand &rand, Who tempDealer, int &die1, int &die2);
    virtual void onDiscarded(const Table &table, Who who);
    virtual void onRiichiEstablished(const Table &table, Who who);
    virtual void onRoundEnded(const Table &table, RoundResult result,
                              const std::vector<Who> &openers, Who gunner,
                              const std::vector<Form> &fs);
    virtual void onIrsChecked(const Table &table, Mount &mount);

    void onFilterChoice(const Table &table, Who who, ChoiceFilter &filter);
    void onActivateDice(const Table &table);
    void onActivate(const Table &table);

    bool irsReady() const;
    const Choices &irsChoices() const;

    virtual void nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                           std::bitset<NUM_NM_SKILL> &presence,
                           const Princess &princess);

    virtual std::string popUpStr() const;

    bool handleIrs(const Table &table, Mount &mount, const Action &action);

protected:
    Girl(Who who, Id id);
    Girl(const Girl &copy);

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
