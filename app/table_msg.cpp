#include "table_msg.h"
#include "../util/string_enum.h"
#include "../util/version.h"



namespace saki
{



using namespace nlohmann;

TableMsgContent::TableMsgContent(const char *event, const json &args)
{
    mJson[field::EVENT] = event;
    mJson[field::ARGS] = args;
}

std::string TableMsgContent::event() const
{
    return mJson[field::EVENT];
}

const json &TableMsgContent::args() const
{
    return mJson[field::ARGS];
}

std::string TableMsgContent::marshal() const
{
    return mJson.dump();
}

void to_json(json &content, const TableMsgContent::JustPause &pause)
{
    json args;
    args["ms"] = pause.ms;

    content[field::EVENT] = "just-pause";
    content[field::ARGS] = args;
}

unsigned createSwapMask(const TileCount &closed,
                        const util::Stactor<T37, 13> &choices)
{
    // assume 'choices' is 34-sorted
    std::bitset<13> mask;
    int i = 0;

    auto it = choices.begin();
    for (const T37 &t : tiles37::ORDER37) {
        if (it == choices.end())
            break;

        int ct = closed.ct(t);
        if (ct > 0) {
            bool val = t.looksSame(*it);
            while (ct-- > 0)
                mask[i++] = val;

            it += val; // consume choice if matched
        }
    }

    return mask.to_ulong();
}

std::string stringOf(const T37 &t, bool lay)
{
    std::string res(t.str());
    if (lay)
        res += '_';

    return res;
}

void to_json(json &array, const util::Range<T37> &ts)
{
    array = json::array();
    for (const T37 &t : ts)
        array.emplace_back(stringOf(t, false));
}

void to_json(json &array, const util::Range<T34> &ts)
{
    array = json::array();
    for (T34 t : ts)
        array.emplace_back(stringOf(T37(t.id34()), false));
}

void to_json(json &obj, const M37 &bark)
{
    using T = M37::Type;
    T type = bark.type();
    obj["type"] = (type == T::CHII ? 1 : (type == T::PON ? 3 : 4));
    int open = bark.layIndex();
    if (type != T::ANKAN)
        obj["open"] = open;

    obj["0"] = stringOf(bark[0], open == 0);
    obj["1"] = stringOf(bark[1], open == 1);
    obj["2"] = stringOf(bark[2], open == 2);

    if (bark.isKan()) {
        obj["3"] = stringOf(bark[3], type == T::KAKAN);
        obj["isDaiminkan"] = (type == T::DAIMINKAN);
        obj["isAnkan"] = (type == T::ANKAN);
        obj["isKakan"] = (type == T::KAKAN);
    }
}

void to_json(json &array, const util::Stactor<M37, 4> &barks)
{
    array = json::array();
    for (const M37 &m : barks)
        array.emplace_back(m);
}

void to_json(json &obj, const IrsCheckItem &item)
{
    obj["modelMono"] = item.mono();
    obj["modelIndent"] = item.indent();
    obj["modelAble"] = item.able();
    obj["modelOn"] = item.on();
}

void to_json(json &obj, const Rule &rule)
{
    obj["fly"] = rule.fly;
    obj["headJump"] = rule.headJump;
    obj["nagashimangan"] = rule.nagashimangan;
    obj["ippatsu"] = rule.ippatsu;
    obj["uradora"] = rule.uradora;
    obj["kandora"] = rule.kandora;
    obj["akadora"] = static_cast<int>(rule.akadora);
    obj["daiminkanPao"] = rule.daiminkanPao;
    obj["hill"] = rule.hill;
    obj["returnLevel"] = rule.returnLevel;
    obj["roundLimit"] = rule.roundLimit;
}

void to_json(json &root, const Replay &replay)
{
    root["version"] = 3;
    root["libVersion"] = VERSION;

    json girls;
    for (saki::Girl::Id v : replay.girls)
        girls.push_back(static_cast<int>(v));

    root["girls"] = girls;

    root["initPoints"] = replay.initPoints;
    root["rule"] = replay.rule;
    root["seed"] = std::to_string(replay.seed);

    json rounds;
    for (const saki::Replay::Round &round : replay.rounds)
        rounds.push_back(round);

    root["rounds"] = rounds;
}

void to_json(json &obj, const Replay::Round &round)
{
    obj["round"] = round.round;
    obj["extraRound"] = round.extraRound;
    obj["dealer"] = round.dealer.index();
    obj["allLast"] = round.allLast;
    obj["deposit"] = round.deposit;
    obj["state"] = std::to_string(round.state);
    obj["die1"] = round.die1;
    obj["die2"] = round.die2;
    obj["result"] = util::stringOf(round.result);
    obj["resultPoints"] = round.resultPoints;

    obj["spells"] = round.spells;
    obj["charges"] = round.charges;

    json drids;
    for (const saki::T37 &t : round.drids)
        drids.push_back(t.str());

    obj["drids"] = drids;

    json urids;
    for (const saki::T37 &t : round.urids)
        urids.push_back(t.str());

    obj["urids"] = urids;

    obj["tracks"] = round.tracks;
}

void to_json(json &obj, const Replay::Track &track)
{
    using str = std::string;

    // *INDENT-OFF*
    auto inJson = [](Replay::InAct inAct) -> str {
        using In = Replay::In;
        switch (inAct.act) {
        case In::DRAW:
            return inAct.t37.str();
        case In::CHII_AS_LEFT: // 'b' means 'begin'
            return str("b") + std::to_string(inAct.showAka5);
        case In::CHII_AS_MIDDLE: // 'm' means 'middle'
            return str("m") + std::to_string(inAct.showAka5);
        case In::CHII_AS_RIGHT: // 'e' means 'end'
            return str("e") + std::to_string(inAct.showAka5);
        case In::PON:
            return str("p") + std::to_string(inAct.showAka5);
        case In::DAIMINKAN:
            return "d";
        case In::RON:
            return "r";
        case In::SKIP_IN:
            return "--";
        default:
            return "err";
        }
    };

    auto outJson = [](saki::Replay::OutAct outAct) -> str {
        using Out = Replay::Out;
        switch (outAct.act) {
        case Out::ADVANCE:
            return outAct.t37.str();
        case Out::SPIN:
            return "->";
        case Out::RIICHI_ADVANCE:
            return str("!") + outAct.t37.str();
        case Out::RIICHI_SPIN:
            return "!->";
        case Out::ANKAN:
            return str("a") + outAct.t37.str();
        case Out::KAKAN:
            return str("k") + outAct.t37.str();
        case Out::RYUUKYOKU:
            return "~";
        case Out::TSUMO:
            return "t";
        case Out::SKIP_OUT:
            return "--";
        default:
            return "err";
        }
    };
    // *INDENT-ON*

    json initArr;
    for (const saki::T37 &t : track.init)
        initArr.push_back(t.str());

    obj["init"] = initArr;

    json inArr;
    for (const saki::Replay::InAct &inAct : track.in)
        inArr.push_back(inJson(inAct));

    obj["in"] = inArr;

    json outArr;
    for (const saki::Replay::OutAct &outAct : track.out)
        outArr.push_back(outJson(outAct));

    obj["out"] = outArr;
}



Action makeAction(const std::string &actStr, int actArg,
                  const std::string &actTile)
{
    using AC = saki::ActCode;
    AC act = util::actCodeOf(actStr.c_str());

    switch (act) {
    case AC::SWAP_OUT:
    case AC::SWAP_RIICHI:
        return Action(act, T37(actTile.c_str()));
    case AC::ANKAN:
        return Action(act, T34(actTile.c_str()));
    case AC::CHII_AS_LEFT:
    case AC::CHII_AS_MIDDLE:
    case AC::CHII_AS_RIGHT:
    case AC::PON:
        return Action(act, actArg, T37(actTile.c_str()));
    case AC::KAKAN:
        return Action(act, static_cast<int>(actArg));
    case AC::IRS_CHECK:
        return Action(act, static_cast<unsigned>(actArg));
    default:
        return Action(act);
    }
}





} // namespace saki
