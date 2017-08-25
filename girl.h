#ifndef SAKI_GIRL_H
#define SAKI_GIRL_H

#include "mount.h"
#include "who.h"
#include "table_observer.h"
#include "choices.h"

#include <bitset>



#define GIRL_CTORS(Name) \
    Name(Who who, Id id) : Girl(who, id) { }; \
    Name(const Name &copy) = default; \
    Name *clone() const override { return new Name(*this); }

#define GIRL_CTORS_CLONE_ONLY(Name) \
    Name(const Name &copy) = default; \
    Name *clone() const override { return new Name(*this); }



namespace saki
{



class Princess;
class Hand;
class Table;
class Choices;
class Action;

struct IrsCheckRow
{
    const bool mono;
    const bool indent;
    const char *name;
    bool able;
    bool on;
};

class Girl
{
public:
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
        INAMURA_KYOUKA = 990001, SHIRATSUKI_SHINO = 990002, HONDOU_YUE = 990003
    };

    enum NmSkill
    {
        WHITE_CLOUD, ZIM_M, ZIM_P, ZIM_S,
        NUM_NM_SKILL
    };

    static Girl *create(Who who, int id);
    virtual Girl *clone() const;
    virtual ~Girl() = default;
    Girl &operator=(const Girl &assign) = delete;

    Id getId() const;

    virtual void onDice(Rand &rand, const Table &table, Choices &choices);
    virtual void onMonkey(std::array<Exist, 4> &exists, const Princess &princess);
    virtual bool checkInit(Who who, const Hand &init, const Princess &princess, int iter);
    virtual void onActivate(const Table &table, Choices &choices);
    virtual void onInbox(Who who, const Action &action);
    virtual void onDraw(const Table &table, Mount &mount, Who who, bool rinshan);
    virtual void onChooseFirstDealer(Rand &rand, Who tempDealer, int &die1, int &die2);
    virtual void onDiscarded(const Table &table, Who who);
    virtual void onRiichiEstablished(const Table &table, Who who);
    virtual void onRoundEnded(const Table &table, RoundResult result,
                              const std::vector<Who> &openers, Who gunner,
                              const std::vector<Form> &fs);

    virtual const IrsCheckRow &irsCheckRow(int index) const;
    virtual int irsCheckCount() const;

    virtual Choices forwardAction(const Table &table, Mount &mount, const Action &action);

    virtual void nonMonkey(Rand &rand, TileCount &init, Mount &mount,
                           std::bitset<NUM_NM_SKILL> &presence,
                           const Princess &princess);


    virtual std::string popUpStr() const;

protected:
    Girl(Who who, Id id);
    Girl(const Girl &copy);

    static void eraseRivered(util::Stactor<T34, 34> &ts, const util::Stactor<T37, 24> &river);
    static void eraseRivered(std::bitset<34> &ts, const util::Stactor<T37, 24> &river);
    void accelerate(Mount &mount, const Hand &hand, const util::Stactor<T37, 24> &river, int delta);

protected:
    const Who mSelf;
    const Id mId;
};



} // namespace saki



#endif // SAKI_GIRL_H


