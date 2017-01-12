#include "girls_senriyama.h"
#include "girls_util_toki.h"
#include "table.h"
#include "ai.h"
#include "string_enum.h"
#include "util.h"

#include <algorithm>
#include <cassert>



namespace saki
{




bool Toki::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    return who != mSelf || iter > 10 || init.step4() <= 4;
}

void Toki::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 25);
}

void Toki::onActivate(const Table &table, TicketFolder &tickets)
{
    (void) table;

    if (mToRiichi)
        return;

    if (tickets.can(ActCode::DICE) || tickets.spinOnly())
        return; // not a branch point

    if (mCd > 0) {
        mCd--;
    } else {
        mCleanTickets = tickets;
        tickets.enable(ActCode::IRS_CLICK);
        mCrazyTickets = tickets;
        mCrazyTickets.setForwarding(true);
    }
}

void Toki::onInbox(Who who, const Action &action)
{
    // range of care, excluding RIICHI, DICE, and IRS
    static const std::vector<ActCode> CHECK {
        ActCode::PASS, ActCode::SWAP_OUT, ActCode::SPIN_OUT,
        ActCode::CHII_AS_LEFT, ActCode::CHII_AS_MIDDLE, ActCode::CHII_AS_RIGHT,
        ActCode::PON, ActCode::DAIMINKAN, ActCode::ANKAN, ActCode::KAKAN,
        ActCode::TSUMO, ActCode::RON, ActCode::RYUUKYOKU
    };

    if (who != mSelf || !mCheckNextAction)
        return;

    if (action.act() == ActCode::RIICHI) {
        mToRiichi = true;
    } else if (util::has(CHECK, action.act())) {
        mCheckNextAction = false;

        std::pair<ActCode, int> move(action.act(), action.encodeArg());
        if (mToRiichi) {
            mToRiichi = false;
            move.first = ActCode::RIICHI; // special mark
        }

        auto it = std::find(mRecords.begin(), mRecords.end(), move);
        mCd = 2 * (it - mRecords.begin());
        mRecords.clear();
    }
}

TicketFolder Toki::forwardAction(const Table &table, Mount &mount, const Action &action)
{
    if (action.act() == ActCode::IRS_CLICK) {
        table.popUp(mSelf, { SkillToken::GREEN });
        if (mInFuture) { // to exit future
            mInFuture = false;
            mCheckNextAction = true;
            return mCleanTickets;
        } else { // to enter future
            mInFuture = true;
            return mCrazyTickets;
        }
    } else if (action.act() == ActCode::RIICHI) {
        TicketFolder tickets;
        tickets.setForwarding(true);

        bool spinnable;
        std::vector<T37> swappables;
        table.getHand(mSelf).declareRiichi(swappables, spinnable);

        tickets.enableSwapOut(swappables);
        if (spinnable)
            tickets.enable(ActCode::SPIN_OUT);

        mToRiichi = true;
        return tickets;
    }

    SkillExpr expr { SkillToken::EMOJI_OO };
    table.popUp(mSelf, expr);

    // prepare operators
    std::array<Girl::Id, 4> ids;
    for (int w = 0; w < 4; w++)
        ids[w] = table.getGirl(Who(w)).getId();
    std::array<std::unique_ptr<TableOperator>, 4> ais = TokiAutoOp::create(ids, action);
    std::array<TableOperator*, 4> operators;
    std::transform(ais.begin(), ais.end(), operators.begin(),
                   [](std::unique_ptr<TableOperator> &up) { return up.get(); });

    // prepare observer
    TokiMountTracker mountTracker(mount, mSelf);
    std::vector<TableObserver*> observers { &mountTracker };

    Table future(table, operators, observers, mSelf, mCleanTickets);
    std::pair<ActCode, int> record(action.act(), action.encodeArg());

    if (mToRiichi) {
        mToRiichi = false;
        record.first = ActCode::RIICHI; // special mark
        future.action(mSelf, Action(ActCode::RIICHI));
    } else {
        future.start();
    }

    // push iff not found
    if (!util::has(mRecords, record))
        mRecords.push_back(record);

    table.popUp(mSelf, mountTracker.getExpr());
    return mCrazyTickets;
}

std::string Toki::stringOf(const SkillExpr &expr) const
{
    if (!expr.empty() && expr[0] == SkillToken::EMOJI_OO)
        return std::string("----  O.O ----");

    if (!expr.empty() && expr[0] == SkillToken::GREEN)
        return std::string("GREEN");

    bool inDiscardStream = false;
    bool toMarkRiichi = false;

    std::ostringstream oss;

    auto it = expr.begin();

    while (it != expr.end()) {
        switch (*it) {
        case SkillToken::DRAW:
            inDiscardStream = false;
            oss << "\nDRAW ";
            it++; // skip 'DRAW' token
            oss << tileOf(*it++);
            break;
        case SkillToken::TMKR:
        case SkillToken::TKR:
            if (!inDiscardStream) {
                oss << "\nDISCARD";
                inDiscardStream = true;
            }
            oss << (*it++ == SkillToken::TMKR ? " *" : " ");
            oss << tileOf(*it++);
            if (toMarkRiichi) {
                oss << "RII";
                toMarkRiichi = false;
            }
            break;
        case SkillToken::CHII: {
            oss << saki::stringOf(barkTypeOf(*it++));
            T37 l = tileOf(*it++);
            T37 m = tileOf(*it++);
            T37 r = tileOf(*it++);
            oss << ' ' << M37::chii(l, m, r, 0); // last 'open=0' dummy/unused
            break;
        }
        case SkillToken::PON: {
            oss << saki::stringOf(barkTypeOf(*it++));
            T37 l = tileOf(*it++);
            T37 m = tileOf(*it++);
            T37 r = tileOf(*it++);
            oss << ' ' << M37::pon(l, m, r, 0); // last 'open=0' dummy/unused
            break;
        }
        case SkillToken::DAIMINKAN:
        case SkillToken::ANKAN:
        case SkillToken::KAKAN:
            oss << saki::stringOf(barkTypeOf(*it++));
            oss << ' ' << tileOf(*it++);
            break;
        case SkillToken::RIICHI:
            toMarkRiichi = true;
            it++;
            break;
        case SkillToken::TSUMO:
        case SkillToken::RON: {
            oss << saki::stringOf(actOf(*it++));
            oss << ' ' << tileOf(*it++);
            std::vector<T37> ts;
            while (it != expr.end() && tokenIsTile(*it))
                ts.push_back(tileOf(*it++));
            oss << '\n' << ts;
            break;
        }
        case SkillToken::KANDORA_I:
        case SkillToken::URADORA_I:
            inDiscardStream = false;
            oss << (*it++ == SkillToken::KANDORA_I ? "\nKANDORAINDIC"
                                                   : "\nURADORAINDIC");
            while (it != expr.end() && tokenIsTile(*it))
                oss << ' ' << tileOf(*it++);
            break;
        case SkillToken::HP: {
            oss << '\n' << saki::stringOf(resultOf(*it++));
            while (it != expr.end()) {
                oss << '\n' << whoOf(*it++).index() << "J TENPAI";
                std::vector<T37> ts;
                while (it != expr.end() && tokenIsTile(*it))
                    ts.push_back(tileOf(*it++));
                oss << '\n' << ts;
            }
            break;
        }
        case SkillToken::KSKP:
        case SkillToken::SFRT:
        case SkillToken::SKSR:
        case SkillToken::SCRC:
        case SkillToken::SCHR:
        case SkillToken::NGSMG:
            oss << '\n' << saki::stringOf(resultOf(*it++));
            break;
        case SkillToken::PLAYER0:
        case SkillToken::PLAYER1:
        case SkillToken::PLAYER2:
        case SkillToken::PLAYER3:
            inDiscardStream = false;
            oss << '\n' << whoOf(*it++).index() << "J ";
            break;
        default:
            unreached("toki expr2str: unhandled token");
            break;
        }
    }

    return oss.str().substr(1); // eliminate first '\n'
}



} // namespace saki


