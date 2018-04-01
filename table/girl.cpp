#include "girl.h"

#include "../girl/achiga_ako.h"
#include "../girl/achiga_kuro.h"
#include "../girl/achiga_yuu.h"
#include "../girl/eisui_hatsumi.h"
#include "../girl/eisui_kasumi.h"
#include "../girl/himematsu_kyouko.h"
#include "../girl/himematsu_suzu.h"
#include "../girl/kiyosumi_nodoka.h"
#include "../girl/kiyosumi_yuuki.h"
#include "../girl/miyamori_toyone.h"
#include "../girl/rinkai_huiyu.h"
#include "../girl/senriyama_sera.h"
#include "../girl/senriyama_toki.h"
#include "../girl/shiraitodai_awai.h"
#include "../girl/shiraitodai_seiko.h"
#include "../girl/shiraitodai_sumire.h"
#include "../girl/shiraitodai_takami.h"
#include "../girl/shiraitodai_teru.h"
#include "../girl/usuzan_sawaya.h"
#include "../girl/x_kazue.h"
#include "../girl/x_kyouka.h"
#include "../girl/x_rio.h"
#include "../girl/x_shino.h"
#include "../girl/x_uta.h"
#include "../girl/x_yue.h"
#include "../girl/x_yui.h"

#include "../util/misc.h"



namespace saki
{



std::unique_ptr<Girl> Girl::create(Who who, Id id)
{
    switch (id) {
    case Id::DOGE:              return std::make_unique<Girl>(who, id);
    case Id::MIYANAGA_TERU:     return std::make_unique<Teru>(who, id);
    case Id::HIROSE_SUMIRE:     return std::make_unique<Sumire>(who, id);
    case Id::SHIBUYA_TAKAMI:    return std::make_unique<Takami>(who, id);
    case Id::MATANO_SEIKO:      return std::make_unique<Seiko>(who, id);
    case Id::OOHOSHI_AWAI:      return std::make_unique<Awai>(who, id);
    case Id::MATSUMI_KURO:      return std::make_unique<Kuro>(who, id);
    case Id::MATSUMI_YUU:       return std::make_unique<Yuu>(who, id);
    case Id::ATARASHI_AKO:      return std::make_unique<Ako>(who, id);
    case Id::ONJOUJI_TOKI:      return std::make_unique<Toki>(who, id);
    case Id::EGUCHI_SERA:       return std::make_unique<Sera>(who, id);
    case Id::USUZUMI_HATSUMI:   return std::make_unique<Hatsumi>(who, id);
    case Id::IWATO_KASUMI:      return std::make_unique<Kasumi>(who, id);
    case Id::ANETAI_TOYONE:     return std::make_unique<Toyone>(who, id);
    case Id::KATAOKA_YUUKI:     return std::make_unique<Yuuki>(who, id);
    case Id::HARAMURA_NODOKA:   return std::make_unique<Nodoka>(who, id);
    case Id::NANPO_KAZUE:       return std::make_unique<Kazue>(who, id);
    case Id::UESHIGE_SUZU:      return std::make_unique<Suzu>(who, id);
    case Id::SUEHARA_KYOUKO:    return std::make_unique<Kyouko>(who, id);
    case Id::SHISHIHARA_SAWAYA: return std::make_unique<Sawaya>(who, id);
    case Id::HAO_HUIYU:         return std::make_unique<Huiyu>(who, id);
    case Id::INAMURA_KYOUKA:    return std::make_unique<Kyouka>(who, id);
    case Id::SHIRATSUKI_SHINO:  return std::make_unique<Shino>(who, id);
    case Id::HONDOU_YUE:        return std::make_unique<Yue>(who, id);
    case Id::MIHOROGI_UTA:      return std::make_unique<Uta>(who, id);
    case Id::TAKUWA_RIO:        return std::make_unique<Rio>(who, id);
    case Id::KAJINO_YUI:        return std::make_unique<Yui>(who, id);
    default:
        unreached("unimplemented girl");
    }
}

Girl::Girl(Who who, Id id)
    : mSelf(who)
    , mId(id)
{
}

std::unique_ptr<Girl> Girl::clone() const
{
    return std::make_unique<Girl>(*this);
}

Girl::Id Girl::getId() const
{
    return mId;
}

///
/// \brief Called when someone is about to dice
///
void Girl::onDice(util::Rand &rand, const Table &table)
{
    (void) rand; (void) table;
}

void Girl::onMonkey(std::array<Exist, 4> &exists, const Table &table)
{
    (void) exists; (void) table;
}

bool Girl::checkInit(Who who, const Hand &init, const Table &table, int iter)
{
    (void) who; (void) init; (void) iter; (void) table; return true;
}

void Girl::onInbox(Who who, const Action &action)
{
    (void) who; (void) action;
}

///
/// \brief Called when someone is about to draw a tile from the mountain
///
void Girl::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table; (void) mount; (void) who; (void) rinshan;
}

void Girl::onChooseFirstDealer(util::Rand &rand, Who tempDealer, int &die1, int &die2)
{
    (void) rand; (void) tempDealer; (void) die1; (void) die2;
}

///
/// \brief Called after self's IRS-check action
///
void Girl::onIrsChecked(const Table &table, Mount &mount)
{
    (void) table; (void) mount;
}

///
/// \brief Called before flipping kandora indicator
///
void Girl::onFlipKandoraIndic(const Table &table, Mount &mount)
{
    (void) table; (void) mount;
}

///
/// \brief Called after someone's win, before digging uradora indicator
/// \param openers Players who opened their hand
///
void Girl::onDigUradoraIndic(const Table &table, Mount &mount, util::Stactor<Who, 4> openers)
{
    (void) table; (void) mount; (void) openers;
}

///
/// \brief Called on the choice-filtering stage
///
void Girl::onFilterChoice(const Table &table, Who who, ChoiceFilter &filter)
{
    filter.join(filterChoice(table, who));
}

///
/// \brief Called when the dealer (self or not) is about to dice
///
void Girl::onActivateDice(const Table &table)
{
    if (!table.beforeEast1())
        mIrsCtrlGetter = attachIrsOnDice();
}

///
/// \brief Called when self is about to be activated
///
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

///
/// \brief Whether the girl is ready to process an IRS action input
/// \return true if ready
///
bool Girl::irsReady() const
{
    return mIrsCtrlGetter.ready();
}

///
/// \brief IRS choice set if IRS-ready, undefined otherwise
/// \return The IRS choice set
///
const Choices &Girl::irsChoices() const
{
    // non-const in the middle, but eventually const
    return mIrsCtrlGetter.get(*const_cast<Girl *>(this)).choices();
}

std::optional<HrhInitFix> Girl::onHrhRaid(const Table &table)
{
    (void) table;
    return std::nullopt;
}

HrhBargainer *Girl::onHrhBargain(const Table &table)
{
    (void) table;
    return nullptr;
}

std::optional<HrhInitFix> Girl::onHrhBeg(util::Rand &rand, const Table &table, const TileCount &stock)
{
    (void) rand;
    (void) table;
    (void) stock;
    return std::nullopt;
}

std::string Girl::popUpStr() const
{
    unreached("unoverriden popUpStr()");
}

///
/// \brief Process an IRS action input
/// \return true if handled, false if the action should be handled by Table
///
bool Girl::handleIrs(const Table &table, Mount &mount, const Action &action)
{
    // guaranteed behavior:
    // clear the attached getter before handling anything
    auto getter = std::move(mIrsCtrlGetter);

    IrsResult res = getter.get(*this).handle(*this, table, mount, action);
    mIrsCtrlGetter = std::move(res.next);
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

///
/// \brief Overriden to define a dice-time IRS's showing condition
/// \return A valid IrsCtrlGetter if an IRS chance should be attached,
///         nullptr if the ordinary Table flow should be taken.
///
Girl::IrsCtrlGetter Girl::attachIrsOnDice()
{
    return nullptr;
}

///
/// \brief Overriden to define a drawn-time IRS's showing condition
/// \return A valid IrsCtrlGetter if an IRS chance should be attached,
///         nullptr if the ordinary Table flow should be taken.
///
Girl::IrsCtrlGetter Girl::attachIrsOnDrawn(const Table &table)
{
    (void) table;
    return nullptr;
}



} // namespace saki
