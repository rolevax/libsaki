#ifndef SAKI_APP_TABLE_MSG_H
#define SAKI_APP_TABLE_MSG_H

#include "replay.h"
#include "../form/hand.h"
#include "../3rd/json.hpp"




namespace saki
{



namespace field
{


constexpr const char *ARGS = "Args";
constexpr const char *EVENT = "Event";

constexpr const char *FORMS = "forms";
constexpr const char *HANDS = "hands";
constexpr const char *OPENERS = "openers";
constexpr const char *PICK = "pick";
constexpr const char *RESULT = "result";



} // namespace field



class TableMsgContent
{
public:
    struct JustPause
    {
        int ms;
    };

    explicit TableMsgContent(const char *event, const nlohmann::json &args);

    template<typename T>
    TableMsgContent(const T &args)
        : mJson(args)
    {
    }

    std::string event() const;
    const nlohmann::json &args() const;
    std::string marshal() const;

private:
    nlohmann::json mJson;
};

void to_json(nlohmann::json &json, const TableMsgContent::JustPause &pause);



struct TableMsg
{
    Who to;
    TableMsgContent content;
};



unsigned createSwapMask(const TileCount &closed,
                        const util::Stactor<T37, 13> &choices);
std::string stringOf(const T37 &t, bool lay = false);
void to_json(nlohmann::json &json, const util::Range<T37> &ts);
void to_json(nlohmann::json &json, const util::Range<T34> &ts);
void to_json(nlohmann::json &json, const M37 &bark);
void to_json(nlohmann::json &json, const util::Stactor<M37, 4> &barks);
void to_json(nlohmann::json &json, const IrsCheckItem &item);
void to_json(nlohmann::json &json, const Rule &rule);
void to_json(nlohmann::json &json, const Replay &replay);
void to_json(nlohmann::json &json, const Replay::Round &round);
void to_json(nlohmann::json &json, const Replay::Track &track);

template<typename T>
std::string marshal(const T &obj)
{
    return nlohmann::json(obj).dump();
}

Action makeAction(const std::string &actStr, int actArg,
                  const std::string &actTile);

} // namespace saki



#endif // SAKI_APP_TABLE_MSG_H
