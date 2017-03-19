#include "tableview.h"
#include "table.h"
#include "form_gb.h"



namespace saki
{



TableView::TableView(const Table &table, Who viewer, TableView::Mode mode)
    : mTable(table)
    , mViewer(viewer)
    , mMode(mode)
{

}

bool TableView::iCan(ActCode act) const
{
    bool res = mTable.getTicketFolder(mViewer).can(act);

    if (res) {
        switch (mMode) { // should use bitset::test in the future
        case Mode::HUIYU_LIMITED:
            if (mMode == Mode::HUIYU_LIMITED) {
                if (act == ActCode::RON) {
                    const T37 &pick = getFocusTile();
                    bool juezhang = mTable.riverRemain(pick) == 0;
                    FormGb f(myHand(), pick, mTable.getPointInfo(mViewer), juezhang);
                    if (f.fan() < 8)
                        res = false;
                } else if (act == ActCode::TSUMO) {
                    bool juezhang = mTable.riverRemain(myHand().drawn()) == 0;
                    FormGb f(myHand(), mTable.getPointInfo(mViewer), juezhang);
                    if (f.fan() < 8)
                        res = false;
                } else if (act == ActCode::RIICHI) {
                    res = false;
                }
                break;
            default:
                break;
            }
        }
    }

    return res;
}

bool TableView::iCanOnlySpin() const
{
    return mTable.getTicketFolder(mViewer).spinOnly();
}

bool TableView::iForwardAny() const
{
    return mTable.getTicketFolder(mViewer).forwardAny();
}

bool TableView::iForwardAll() const
{
    return mTable.getTicketFolder(mViewer).forwardAll();
}

std::vector<Action> TableView::myChoices() const
{
    std::vector<Action> res;

    for (int a = 0; a < ActCode::NUM_ACTCODE; a++) {
        ActCode act = static_cast<ActCode>(a);
        if (act != ActCode::NOTHING && !iCan(act))
            continue;

        switch (act) {
        case PASS:
        case SPIN_OUT:
        case DAIMINKAN:
        case RIICHI:
        case TSUMO:
        case RON:
        case RYUUKYOKU:
        case END_TABLE:
        case NEXT_ROUND:
        case DICE:
            res.emplace_back(act);
            break;
        case SWAP_OUT:
            for (const T37 &out : mySwappables())
                res.emplace_back(act, out);
            break;
        case ANKAN:
            for (T34 k : myAnkanables())
                res.emplace_back(act, T37(k.id34()));
            break;
        case KAKAN:
            for (int barkId : myKakanables())
                res.emplace_back(act, barkId);
            break;
        case CHII_AS_LEFT:
        case CHII_AS_MIDDLE:
        case CHII_AS_RIGHT:
        case PON: // currently does not consider showAka5 < 2 cases
            res.emplace_back(act, 2);
            break;
        case IRS_CHECK:
        case IRS_CLICK:
        case IRS_RIVAL: // skip forwarded actions
        case NOTHING: // skip nothing
            break;
        default:
            unreached("TicketFolder::choices");
        }
    }

    return res;
}

Action TableView::mySweep() const
{
    return mTable.getTicketFolder(mViewer).sweep();
}

const std::vector<T37> &TableView::mySwappables() const
{
    return mTable.getTicketFolder(mViewer).swappables();
}

const std::vector<T34> &TableView::myAnkanables() const
{
    return mTable.getTicketFolder(mViewer).ankanables();
}

const std::vector<int> &TableView::myKakanables() const
{
    return mTable.getTicketFolder(mViewer).kakanables();
}

const Girl &TableView::me() const
{
    return mTable.getGirl(mViewer);
}

const Hand &TableView::myHand() const
{
    return mTable.getHand(mViewer);
}

int TableView::myRank() const
{
    return mTable.getRank(mViewer);
}

int TableView::getRound() const
{
    return mTable.getRound();
}

Who TableView::getDealer() const
{
    return mTable.getDealer();
}

const TableFocus &TableView::getFocus() const
{
    return mTable.getFocus();
}

const T37 &TableView::getFocusTile() const
{
    return mTable.getFocusTile();
}

const std::vector<T37> &TableView::getDrids() const
{
    return mTable.getMount().getDrids();
}

const RuleInfo &TableView::getRuleInfo() const
{
    return mTable.getRuleInfo();
}

int TableView::getSelfWind() const
{
    return mTable.getSelfWind(mViewer);
}

int TableView::getRoundWind() const
{
    return mTable.getRoundWind();
}

TileCount TableView::visibleRemain() const
{
    return mTable.visibleRemain(mViewer);
}

Who TableView::findGirl(Girl::Id id) const
{
    return mTable.findGirl(id);
}

const std::vector<T37> &TableView::getRiver(Who who) const
{
    return mTable.getRiver(who);
}

const std::vector<M37> &TableView::getBarks(Who who) const
{
    return mTable.getHand(who).barks();
}

bool TableView::genbutsu(Who whose, T34 t) const
{
    return mTable.genbutsu(whose, t);
}

bool TableView::riichiEstablished(Who who) const
{
    return mTable.riichiEstablished(who);
}

bool TableView::isMenzen(Who who) const
{
    return mTable.getHand(who).isMenzen();
}

bool TableView::isAllLast() const
{
    return mTable.isAllLast();
}

bool TableView::inIppatsuCycle() const
{
    return mTable.inIppatsuCycle();
}



} // namespace saki


