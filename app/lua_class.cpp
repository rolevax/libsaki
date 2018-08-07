#include "lua_class.h"
#include "../table/table.h"

namespace saki
{



void setupLuaClasses(sol::environment env, LuaUserErrorHandler &error)
{
    setupLuaTile(env, error);
    setupLuaWho(env);
    setupLuaMeld(env);
    setupLuaMount(env);
    setupLuaTileCount(env);
    setupLuaHand(env);
    setupLuaGame(env);
}

void setupLuaTile(sol::environment env, LuaUserErrorHandler &error)
{
    env.new_enum<Suit>(
        "Suit", {
            { "M", Suit::M },
            { "P", Suit::P },
            { "S", Suit::S },
            { "F", Suit::F },
            { "Y", Suit::Y }
        }
    );

    env.new_usertype<T34>(
        "T34",
        sol::meta_function::construct, sol::factories(
            [&error](int ti) {
                if (ti < 0 || ti >= 34) {
                    error.handleUserError("invalid T34 id");
                    return T34();
                }

                return T34(ti);
            },
            [&error](Suit s, int v) {
                if (!(1 <= v && v <= 9)) {
                    error.handleUserError("invalid T34 val");
                    return T34();
                }

                return T34(s, v);
            },
            [&error](const std::string s) {
                static const std::array<std::string, 34> dict {
                    "1m", "2m", "3m", "4m", "5m", "6m", "7m", "8m", "9m",
                    "1p", "2p", "3p", "4p", "5p", "6p", "7p", "8p", "9p",
                    "1s", "2s", "3s", "4s", "5s", "6s", "7s", "8s", "9s",
                    "1f", "2f", "3f", "4f", "1y", "2y", "3y"
                };

                auto it = std::find(dict.begin(), dict.end(), s);
                if (it == dict.end()) {
                    error.handleUserError("invalid T34 string");
                    return T34();
                }

                return T34(it - dict.begin());
            }
        ),
        "id34", &T34::id34,
        "isyakuhai", &T34::isYakuhai,
        sol::meta_function::to_string, &T34::str,
        "all", sol::var(std::vector<T34>(tiles34::ALL34.begin(), tiles34::ALL34.end()))
    );
}

void setupLuaWho(sol::environment env)
{
    env.new_usertype<Who>(
        "Who",
        "right", &Who::right,
        "cross", &Who::cross,
        "left", &Who::left,
        sol::meta_function::to_string, &Who::index,
        sol::meta_function::equal_to, &Who::operator==
    );
}

void setupLuaMeld(sol::environment env)
{
    env.new_usertype<M37>(
        "M37",
        "type", &M37::type,
        sol::meta_function::index, &M37::operator[]
    );

    sol::table m37 = env["M37"];
    m37.new_enum<M37::Type>(
        "Type", {
            { "CHII", M37::Type::CHII },
            { "PON", M37::Type::PON },
            { "DAIMINKAN", M37::Type::DAIMINKAN },
            { "ANKAN", M37::Type::ANKAN },
            { "KAKAN", M37::Type::KAKAN }
        }
    );
}

void setupLuaMount(sol::environment env)
{
    env.new_usertype<Mount>(
        "Mount",
        "remainpii", &Mount::remainPii,
        "remainrinshan", &Mount::remainRinshan,
        "remaina", sol::overload(
            [](Mount &mount, T34 t) {
                mount.remainA(t);
            },
            [](Mount &mount, const T37 &t) {
                mount.remainA(t);
            }
        ),
        "getdrids", &Mount::getDrids,
        "geturids", &Mount::getUrids,
        "lighta", sol::overload(
            [](Mount &mount, T34 t, int mk, bool rin) {
                mount.lightA(t, mk, rin);
            },
            [](Mount &mount, T34 t, int mk) {
                mount.lightA(t, mk);
            }
        ),
        "lightb", sol::overload(
            [](Mount &mount, T34 t, int mk, bool rin) {
                mount.lightB(t, mk, rin);
            },
            [](Mount &mount, T34 t, int mk) {
                mount.lightB(t, mk);
            }
        ),
        "incmk", sol::overload(
            [](Mount &mount, Mount::Exit exit, size_t pos, T34 t, int delta, bool bSpace) {
                mount.incMk(exit, pos, t, delta, bSpace);
            },
            [](Mount &mount, Mount::Exit exit, size_t pos, const T37 &t, int delta, bool bSpace) {
                mount.incMk(exit, pos, t, delta, bSpace);
            }
        ),
        "loadB", &Mount::loadB,
        "dump", [](Mount &mount){
            (void) mount;
            // TODO return a table of mk
        }
    );

    sol::table m37 = env["Mount"];
    m37.new_enum<Mount::Exit>(
        "Exit", {
            { "PII", Mount::Exit::PII },
            { "RINSHAN", Mount::Exit::RINSHAN },
            { "DORAHYOU", Mount::Exit::DORAHYOU },
            { "URAHYOU", Mount::Exit::URAHYOU }
        }
    );
}

void setupLuaTileCount(sol::environment env)
{
    env.new_usertype<TileCount>(
        "Tilecount",
        "ct", sol::overload(
            [](const TileCount &tc, T34 t) {
                return tc.ct(t);
            },
            [](const TileCount &tc, Suit s) {
                return tc.ct(s);
            }
        )
    );
}

void setupLuaHand(sol::environment env)
{
    env.new_usertype<Hand>(
        "Hand",
        "closed", &Hand::closed,
        "ct", &Hand::ct,
        "ready", &Hand::ready,
        "step", &Hand::step,
        "step4", &Hand::step4,
        "step7", &Hand::step7,
        "step13", &Hand::step13,
        "effa", &Hand::effA,
        "effa4", &Hand::effA4,
        "ismenzen", &Hand::isMenzen,
        "barks", &Hand::barks
    );
}

void setupLuaGame(sol::environment env)
{
    env.new_usertype<Table>(
        "Game",
        "gethand", &Table::getHand,
        "getround", &Table::getRound,
        "getextraround", &Table::getExtraRound,
        "getdealer", &Table::getDealer,
        "getselfwind", &Table::getSelfWind,
        "getroundwind", &Table::getRoundWind
    );
}



} // namespace saki
