#include "girl.h"

#include "../girl/girls_shiraitodai.h"
#include "../girl/girls_achiga.h"
#include "../girl/girls_senriyama.h"
#include "../girl/girls_eisui.h"
#include "../girl/girls_miyamori.h"
#include "../girl/girls_kiyosumi.h"
#include "../girl/girls_himematsu.h"
#include "../girl/girls_usuzan.h"
#include "../girl/girls_rinkai.h"
#include "../girl/girls_asakumi.h"
#include "../girl/girls_other.h"

#include "../util/misc.h"



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
    case Id::HAO_HUIYU:         return new Huiyu(who, id);
    case Id::INAMURA_KYOUKA:    return new Kyouka(who, id);
    case Id::SHIRATSUKI_SHINO:  return new Shino(who, id);
    case Id::HONDOU_YUE:        return new Yue(who, id);
    case Id::MIHOROGI_UTA:      return new Uta(who, id);
    case Id::TAKUWA_RIO:        return new Rio(who, id);
    case Id::KAJINO_YUI:        return new Yui(who, id);
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
    : TableObserver(copy)
    , mSelf(copy.mSelf)
    , mId(copy.mId)
{
}

Girl::Id Girl::getId() const
{
    return mId;
}

void Girl::onDice(util::Rand &rand, const Table &table)
{
    (void) rand; (void) table;
}

void Girl::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) exists; (void) princess;
}

bool Girl::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) who; (void) init; (void) iter; (void) princess; return true;
}

void Girl::onInbox(Who who, const Action &action)
{
    (void) who; (void) action;
}

void Girl::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table; (void) mount; (void) who; (void) rinshan;
}

void Girl::onChooseFirstDealer(util::Rand &rand, Who tempDealer, int &die1, int &die2)
{
    (void) rand; (void) tempDealer; (void) die1; (void) die2;
}

void Girl::onIrsChecked(const Table &table, Mount &mount)
{
    (void) table; (void) mount;
}

void Girl::onFilterChoice(const Table &table, Who who, ChoiceFilter &filter)
{
    filter.join(filterChoice(table, who));
}

void Girl::onActivateDice(const Table &table)
{
    if (!table.beforeEast1())
        mIrsCtrlGetter = attachIrsOnDice();
}

void Girl::onActivate(const Table &table)
{
    const Choices &choices = table.getChoices(mSelf);
    switch (choices.mode()) {
    case Choices::Mode::DRAWN:
        mIrsCtrlGetter = attachIrsOnDrawn(table);
        break;
    default:
        break;
    }
}

bool Girl::irsReady() const
{
    return mIrsCtrlGetter.ready();
}

const Choices &Girl::irsChoices() const
{
    // non-const in the middle, but eventually const
    return mIrsCtrlGetter.get(*const_cast<Girl *>(this)).choices();
}

void Girl::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                     std::bitset<Girl::NUM_NM_SKILL> &presence,
                     const Princess &princess)
{
    (void) rand; (void) init; (void) mount; (void) presence; (void) princess;
    unreached("unoverriden nonMonkey()");
}

std::string Girl::popUpStr() const
{
    unreached("unoverriden popUpStr()");
}

bool Girl::handleIrs(const Table &table, Mount &mount, const Action &action)
{
    auto icg = mIrsCtrlGetter;
    mIrsCtrlGetter = nullptr;
    IrsResult res = icg.get(*this).handle(*this, table, mount, action);
    mIrsCtrlGetter = res.next;
    return res.handled;
}

void Girl::eraseRivered(util::Stactor<T34, 34> &ts, const River &river)
{
    // 34/37 does not affect equalty
    auto has = [river](T34 t) { return util::has(river, T37(t.id34())); };
    std::remove_if(ts.begin(), ts.end(), has);
}

void Girl::eraseRivered(std::bitset<34> &ts, const River &river)
{
    for (const T37 &t: river)
        ts.reset(t.id34());
}

void Girl::accelerate(Mount &mount, const Hand &hand, const River &river, int delta)
{
    util::Stactor<T34, 34> effs = hand.effA4();

    // do not draw dropped tiles
    // in order to enable user-controled advance direction
    eraseRivered(effs, river);

    for (T34 t : effs)
        mount.lightA(t, delta);
}

ChoiceFilter Girl::filterChoice(const Table &table, Who who)
{
    (void) table; (void) who;
    return ChoiceFilter();
}

Girl::IrsCtrlGetter Girl::attachIrsOnDice()
{
    return nullptr;
}

Girl::IrsCtrlGetter Girl::attachIrsOnDrawn(const Table &table)
{
    (void) table;
    return nullptr;
}



} // namespace saki
