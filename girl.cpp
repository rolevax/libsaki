#include "girl.h"

#include "girls_shiraitodai.h"
#include "girls_achiga.h"
#include "girls_senriyama.h"
#include "girls_eisui.h"
#include "girls_miyamori.h"
#include "girls_kiyosumi.h"
#include "girls_himematsu.h"
#include "girls_usuzan.h"
#include "girls_rinkai.h"
#include "girls_asakumi.h"
#include "girls_other.h"

#include "util.h"



namespace saki
{



Girl *Girl::create(Who who, int id_)
{
    Id id = Id(id_);
    switch (id) {
    case Id::DOGE:              return new Girl(who, id);
    case Id::MIYANAGA_TERU:     return new Teru(who, id);
    case Id::HIROSE_SUMIRE:     return new Sumire(who, id);
    case Id::SHIBUYA_TAKAMI:    return new Takami(who, id);
    case Id::MATANO_SEIKO:      return new Seiko(who, id);
    case Id::OOHOSHI_AWAI:      return new Awai(who, id);
    case Id::MATSUMI_KURO:      return new Kuro(who, id);
    case Id::MATSUMI_YUU:       return new Yuu(who, id);
    case Id::ATARASHI_AKO:      return new Ako(who, id);
    case Id::ONJOUJI_TOKI:      return new Toki(who, id);
    case Id::EGUCHI_SERA:       return new Sera(who, id);
    case Id::USUZUMI_HATSUMI:   return new Hatsumi(who, id);
    case Id::IWATO_KASUMI:      return new Kasumi(who, id);
    case Id::ANETAI_TOYONE:     return new Toyone(who, id);
    case Id::KATAOKA_YUUKI:     return new Yuuki(who, id);
    case Id::HARAMURA_NODOKA:   return new Nodoka(who, id);
    case Id::NANPO_KAZUE:       return new Kazue(who, id);
    case Id::UESHIGE_SUZU:      return new Suzu(who, id);
    case Id::SUEHARA_KYOUKO:    return new Kyouko(who, id);
    case Id::SHISHIHARA_SAWAYA: return new Sawaya(who, id);
    case Id::HAO_HUIYU: 		return new Huiyu(who, id);
    case Id::INAMURA_KYOUKA:    return new Kyouka(who, id);
    case Id::SHIRATSUKI_SHINO:  return new Shino(who, id);
    case Id::HONDOU_YUE:        return new Yue(who, id);
    default:
        unreached("unimplemented girl");
    }
}

Girl *Girl::clone() const
{
    return new Girl(*this);
}

Girl::Girl(Who who, Id id)
    : mSelf(who)
    , mId(id)
{
}

Girl::Girl(const Girl &copy)
    : mSelf(copy.mSelf)
    , mId(copy.mId)
{
}

Girl::Id Girl::getId() const
{
    return mId;
}

void Girl::onDice(Rand &rand, const Table &table, Choices &choices)
{
    (void) rand; (void) table; (void) choices;
}

void Girl::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) exists; (void) princess;
}

bool Girl::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) who; (void) init; (void) iter; (void) princess; return true;
}

void Girl::onActivate(const Table &table, Choices &choices)
{
    (void) table; (void) choices;
}

void Girl::onInbox(Who who, const Action &action)
{
    (void) who; (void) action;
}

void Girl::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table; (void) mount; (void) who; (void) rinshan;
}

void Girl::onChooseFirstDealer(Rand &rand, Who tempDealer, int &die1, int &die2)
{
    (void) rand; (void) tempDealer; (void) die1; (void) die2;
}

void Girl::onDiscarded(const Table &table, Who who)
{
    (void) table; (void) who;
}

void Girl::onRiichiEstablished(const Table &table, Who who)
{
    (void) table; (void) who;
}

void Girl::onRoundEnded(const Table &table, RoundResult result,
                        const std::vector<Who> &openers, Who gunner,
                        const std::vector<Form> &fs)
{
    (void) table; (void) result; (void) openers; (void) gunner; (void) fs;
}

void Girl::nonMonkey(Rand &rand, TileCount &init, Mount &mount,
                     std::bitset<Girl::NUM_NM_SKILL> &presence,
                     const Princess &princess)
{
    (void) rand; (void) init; (void) mount; (void) presence; (void) princess;
    unreached("unoverriden nonMonkey()");
}

const IrsCheckRow &Girl::irsCheckRow(int index) const
{
    (void) index;
    unreached("unoverriden getIrsCheckRow()");
}

int Girl::irsCheckCount() const
{
    unreached("unoverriden irsCheckCount()");
    return 0;
}

Choices Girl::forwardAction(const Table &table, Mount &mount, const Action &action)
{
    (void) table; (void) mount; (void) action;
    unreached("unoverriden forwardAction()");
}

std::string Girl::popUpStr() const
{
    unreached("unoverriden popUpStr()");
}

void Girl::eraseRivered(util::Stactor<T34, 34> &ts, const util::Stactor<T37, 24> &river)
{
    // 34/37 does not affect equalty
    auto has = [river](T34 t){ return util::has(river, T37(t.id34())); };
    std::remove_if(ts.begin(), ts.end(), has);
}

void Girl::eraseRivered(std::bitset<34> &ts, const util::Stactor<T37, 24> &river)
{
    for (const T37 &t: river)
        ts.reset(t.id34());
}

void Girl::accelerate(Mount &mount, const Hand &hand, const util::Stactor<T37, 24> &river, int delta)
{
    util::Stactor<T34, 34> effs = hand.effA4();

    // do not draw dropped tiles
    // in order to enable user-controled advance direction
    eraseRivered(effs, river);

    for (T34 t : effs)
        mount.lightA(t, delta);
}



} // namespace saki


