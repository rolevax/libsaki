#include "lua_class.h"
#include "lua_dream_hand.h"
#include "../table/table.h"
#include "../util/string_enum.h"

namespace saki
{



bool isValidSuitStr(const std::string &s)
{
    return s.size() == 1 && T34::isValidSuit(s[0]);
}

std::string toLower(const char *s)
{
    std::string res(s);
    std::for_each(res.begin(), res.end(), [](char &c) {
        c = static_cast<char>(std::tolower(c));
    });
    return res;
}

std::pair<Mount::Exit, bool> parseMountExit(const std::string &s)
{
    std::pair<Mount::Exit, bool> res;

    res.second = true;
    if (s == "pii")
        res.first = Mount::Exit::PII;
    else if (s == "rinshan")
        res.first = Mount::Exit::RINSHAN;
    else if (s == "dorahyou")
        res.first = Mount::Exit::DORAHYOU;
    else if (s == "urahyou")
        res.first = Mount::Exit::URAHYOU;
    else
        res.second = false;

    return res;
}

template<typename Class, typename Ret, typename... Args>
class AsTable
{
public:
    using Method = Ret (Class::*)(Args...) const;
    using Table = sol::as_table_t<sol::meta::unqualified_t<Ret>>;

    explicit AsTable(Method method)
        : mMethod(method)
    {
    }

    Table operator()(Class &thiz, Args... args)
    {
        return sol::as_table((thiz.*mMethod)(args...));
    }

private:
    Method mMethod;
};

template<typename Class, typename Ret, typename... Args>
class ReturnCopy
{
public:
    using Method = Ret (Class::*)(Args...) const;

    explicit ReturnCopy(Method method)
        : mMethod(method)
    {
    }

    std::remove_reference_t<Ret> operator()(Class &thiz, Args &&... args)
    {
        return (thiz.*mMethod)(std::forward<Args>(args)...);
    }

private:
    Method mMethod;
};

int handMod1(sol::table ids, const Hand &hand)
{
    int ct = 0;
    for (const auto &[key, ref] : ids) {
        (void) ref; // conv-to-opt doesn't work somehow

        std::optional<T34> id = ids[key];
        if (id == std::nullopt) {
            std::optional<T37> id37 = ids[key];
            id = id37;
        }

        if (id != std::nullopt)
            ct += *id % hand;
    }

    return ct;
}

int handMod2(T34 indic, const Hand &hand)
{
    return indic % hand;
}

void setupLuaClasses(const sol::environment &env, LuaUserErrorHandler &error)
{
    setupLuaTile(env, error);
    setupLuaWho(env);
    setupLuaMeld(env, error);
    setupLuaExist(env, error);
    setupLuaMount(env, error);
    setupLuaTileCount(env, error);
    setupLuaHand(env);
    setupLuaDreamHand(env, error);
    setupLuaForm(env, error);
    setupLuaRule(env);
    setupLuaFormCtx(env);
    setupLuaGame(env);
}

void setupLuaTile(sol::environment env, LuaUserErrorHandler &error)
{
    env.new_usertype<T34>(
        "T34",
        sol::meta_function::construct, sol::factories(
            [&error](int ti) {
                if (ti < 0 || ti >= 34) {
                    error.handleUserError("EInvT34Id");
                    return T34();
                }

                return T34(ti);
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
                    error.handleUserError("EInvT34Str");
                    return T34();
                }

                return T34(static_cast<int>(it - dict.begin()));
            }
        ),
        "id34", &T34::id34,
        "suit", [](T34 t) { return T34::charOf(t.suit()); },
        "val", &T34::val,
        "str34", &T34::str34,
        "isz", &T34::isZ,
        "isnum", &T34::isNum,
        "isnum19", &T34::isNum19,
        "isyao", &T34::isYao,
        "isyakuhai", &T34::isYakuhai,
        "dora", &T34::dora,
        "indicator", &T34::indicator,
        sol::meta_function::to_string, &T34::str34,
        sol::meta_function::equal_to, &T34::operator==,
        sol::meta_function::less_than, &T34::operator<,
        sol::meta_function::modulus, &T34::operator%,
        "all", sol::var(std::vector<T34>(tiles34::ALL34.begin(), tiles34::ALL34.end()))
    );

    env.new_usertype<T37>(
        "T37",
        sol::meta_function::construct, sol::factories(
            [&error](int ti) {
                if (ti < 0 || ti >= 34) {
                    error.handleUserError("EInvT34Id");
                    return T37();
                }

                return T37(ti);
            },
            [&error](const std::string s) {
                if (!T37::isValidStr(s.c_str())) {
                    error.handleUserError("EInvT37Str");
                    return T37();
                }
                return T37(s.c_str());
            }
        ),
        "isaka5", &T37::isAka5,
        "lookssame", &T37::looksSame,
        "str37", &T37::str37,
        sol::meta_function::to_string, &T37::str37,
        sol::base_classes, sol::bases<T34>()
    );
}

void setupLuaWho(sol::environment env)
{
    env.new_usertype<Who>(
        "Who",
        "right", &Who::right,
        "cross", &Who::cross,
        "left", &Who::left,
        "bydice", &Who::byDice,
        "byturn", &Who::byTurn,
        "looksat", &Who::looksAt,
        "turnfrom", &Who::turnFrom,
        "index", [](Who w) { return w.index() + 1; },
        sol::meta_function::to_string, [](Who w) { return w.index() + 1; },
        sol::meta_function::equal_to, &Who::operator==
    );
}

void setupLuaMeld(sol::environment env, LuaUserErrorHandler &error)
{
    env.new_usertype<M37>(
        "M37",
        "type", [](const M37 &m) {
            return toLower(util::stringOf(m.type()));
        },
        "has", &M37::has,
        sol::meta_function::index, [&error](const M37 &m, int index) {
            int zeroIndex = index - 1;
            int size = static_cast<int>(m.tiles().size());
            if (zeroIndex < 0 || zeroIndex > size) {
                error.handleUserError("EInvM37Idx");
                return T37();
            }

            return m[index];
        }
    );
}

void setupLuaExist(sol::environment env, LuaUserErrorHandler &error)
{
    (void) error;
    env.new_usertype<Exist>(
        "Exist",
        "incmk", sol::overload(
            [](Exist &exist, T34 t, int delta) {
                exist.incMk(t, delta);
            },
            [](Exist &exist, const T37 &t, int delta) {
                exist.incMk(t, delta);
            }
        )
    );
}

void setupLuaMount(sol::environment env, LuaUserErrorHandler &error)
{
    env.new_usertype<Mount>(
        "Mount",
        "remainpii", &Mount::remainPii,
        "remainrinshan", &Mount::remainRinshan,
        "remaina", sol::overload(
            [](Mount &mount, T34 t) {
                return mount.remainA(t);
            },
            [](Mount &mount, const T37 &t) {
                return mount.remainA(t);
            }
        ),
        "getdrids", AsTable(&Mount::getDrids),
        "geturids", AsTable(&Mount::getUrids),
        "lighta", sol::overload(
            [](Mount &mount, T34 t, int mk, bool rin) {
                mount.lightA(t, mk, rin);
            },
            [](Mount &mount, const T37 &t, int mk, bool rin) {
                mount.lightA(t, mk, rin);
            },
            [](Mount &mount, T34 t, int mk) {
                mount.lightA(t, mk);
            },
            [](Mount &mount, const T37 &t, int mk) {
                mount.lightA(t, mk);
            }
        ),
        "lightb", sol::overload(
            [](Mount &mount, T34 t, int mk, bool rin) {
                mount.lightB(t, mk, rin);
            },
            [](Mount &mount, const T37 &t, int mk, bool rin) {
                mount.lightB(t, mk, rin);
            },
            [](Mount &mount, T34 t, int mk) {
                mount.lightB(t, mk);
            },
            [](Mount &mount, const T37 &t, int mk) {
                mount.lightB(t, mk);
            }
        ),
        "incmk", sol::overload(
            [&error](Mount &mount, std::string exit, size_t pos, T34 t, int delta, bool bSpace) {
                auto [e, ok] = parseMountExit(exit);
                if (!ok) {
                    error.handleUserError("EInvMntExt");
                    return;
                }
                mount.incMk(e, pos, t, delta, bSpace);
            },
            [&error](Mount &mount, std::string exit, size_t pos, const T37 &t, int delta, bool bSpace) {
                auto [e, ok] = parseMountExit(exit);
                if (!ok) {
                    error.handleUserError("EInvMntExt");
                    return;
                }
                mount.incMk(e, pos, t, delta, bSpace);
            }
        ),
        "loadb", &Mount::loadB
    );
}

void setupLuaTileCount(sol::environment env, LuaUserErrorHandler &error)
{
    env.new_usertype<TileCount>(
        "Tilecount",
        "ct", sol::overload(
            [](const TileCount &tc, T34 t) {
                return tc.ct(t);
            },
            [](const TileCount &tc, const T37 &t) {
                return tc.ct(t);
            },
            [&error](const TileCount &tc, std::string suit) {
                if (!isValidSuitStr(suit)) {
                    error.handleUserError("EInvSuit");
                    return 0;
                }

                return tc.ct(T34::suitOf(suit[0]));
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
        "ctaka5", &Hand::ctAka5,
        "ready", &Hand::ready,
        "step", &Hand::step,
        "step4", &Hand::step4,
        "step7", &Hand::step7,
        "step7gb", &Hand::step7Gb,
        "step13", &Hand::step13,
        "effa", AsTable(&Hand::effA),
        "effa4", AsTable(&Hand::effA4),
        "ismenzen", &Hand::isMenzen,
        "barks", AsTable(&Hand::barks),
        "canchii", &Hand::canChii,
        "canchiiasleft", &Hand::canChiiAsLeft,
        "canchiiasmiddle", &Hand::canChiiAsMiddle,
        "canchiiasright", &Hand::canChiiAsRight,
        "canpon", &Hand::canPon,
        "candaiminkan", &Hand::canDaiminkan,
        sol::meta_function::modulus, sol::overload(handMod1, handMod2)
    );
}

void setupLuaDreamHand(sol::environment env, LuaUserErrorHandler &error)
{
    env.new_usertype<LuaDreamHand>(
        "Dreamhand",
        sol::meta_function::construct, [&error](const Hand &hand) {
            return LuaDreamHand(hand, error);
        },
        "draw", &LuaDreamHand::safeDraw,
        "swapout", &LuaDreamHand::safeSwapOut,
        "spinout", &LuaDreamHand::safeSpinOut,
        sol::base_classes, sol::bases<Hand>()
    );
}

void setupLuaForm(sol::environment env, LuaUserErrorHandler &error)
{
    env.new_usertype<Form>(
        "Form",
        sol::meta_function::construct, sol::factories(
            [&error](const Hand &full, const FormCtx &ctx, const Rule &rule) -> sol::optional<Form> {
                if (full.step() != -1) {
                    error.handleUserError("EFrmNoAgr");
                    return sol::nullopt;
                }

                return Form(full, ctx, rule);
            },
            [&error](const Hand &ready, const T37 &pick, const FormCtx &ctx, const Rule &rule) -> sol::optional<Form> {
                if (ready.peekPickStep(pick) != -1) {
                    error.handleUserError("EFrmNoAgr");
                    return sol::nullopt;
                }

                return Form(ready, pick, ctx, rule);
            }
        ),
        "isprototypalyakuman", &Form::isPrototypalYakuman,
        "fu", &Form::fu,
        "han", &Form::han,
        "base", &Form::base,
//        "dora", &Form::dora,
//        "uradora", &Form::uradora,
//        "akadora", &Form::akadora,
        "yakus", [&env](const Form &f) {
            sol::table set = env.create();
            for (const char *key : f.keys())
                set[key] = true;

            return set;
        },
        "hasyaku", &Form::hasYaku,
        "netloss", &Form::netLoss,
        "netgain", &Form::netGain,
        "loss", &Form::loss,
        "gain", &Form::gain,
        "spell", &Form::spell,
        "charge", &Form::charge
    );
}

void setupLuaRule(sol::environment env)
{
    env.new_usertype<Rule>(
        "Rule",
        "fly", &Rule::fly,
        "headjump", &Rule::headJump,
        "nagashimangan", &Rule::nagashimangan,
        "ippatsu", &Rule::ippatsu,
        "uradora", &Rule::uradora,
        "kandora", &Rule::kandora,
        "daiminkanpao", &Rule::daiminkanPao,
        "akadora", sol::property(
            [](const Rule &r) {
                return r.akadora == TileCount::AKADORA0 ? 0
                     : r.akadora == TileCount::AKADORA3 ? 3 : 4;
            },
            [](Rule &r, int a) {
                r.akadora = a == 0 ? TileCount::AKADORA0
                          : a == 3 ? TileCount::AKADORA3 : TileCount::AKADORA4;
            }
        ),
        "hill", &Rule::hill,
        "returnlevel", &Rule::returnLevel,
        "roundlimit", &Rule::roundLimit
    );
}

void setupLuaFormCtx(sol::environment env)
{
    env.new_usertype<FormCtx>(
        "Formctx",
        "ippatsu", &FormCtx::ippatsu,
        "bless", &FormCtx::bless,
        "duringkan", &FormCtx::duringKan,
        "emptymount", &FormCtx::emptyMount,
        "riichi", &FormCtx::riichi,
        "roundwind", &FormCtx::roundWind,
        "selfwind", &FormCtx::selfWind,
        "extraround", &FormCtx::extraRound
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
        "getroundwind", &Table::getRoundWind,
        "getriver", AsTable(&Table::getRiver),
        "riichiestablished", &Table::riichiEstablished,
        "getrule", ReturnCopy(&Table::getRule),
        "getformctx", &Table::getFormCtx
    );
}

sol::table toLuaTable(sol::environment env, const TableEvent &event)
{
    using TE = TableEvent;

    sol::table args = env.create();

    switch (event.type()) {
    case TE::Type::TABLE_STARTED:
        args["seed"] = event.as<TE::TableStarted>().seed;
        break;
    case TE::Type::FIRST_DEALER_CHOSEN:
        args["who"] = event.as<TE::FirstDealerChosen>().who;
        break;
    case TE::Type::ROUND_STARTED: {
        const auto &a = event.as<TE::RoundStarted>();
        args["round"] = a.round;
        args["extraround"] = a.extraRound;
        args["dealer"] = a.dealer;
        args["alllast"] = a.allLast;
        args["deposit"] = a.deposit;
        args["seed"] = a.seed;
        break;
    }
    case TE::Type::DICED: {
        const auto &a = event.as<TE::Diced>();
        args["die1"] = a.die1;
        args["die2"] = a.die2;
        break;
    }
    case TE::Type::DRAWN:
        args["who"] = event.as<TE::Drawn>().who;
        break;
    case TE::Type::DISCARDED:
        args["spin"] = event.as<TE::Discarded>().spin;
        break;
    case TE::Type::RIICHI_CALLED:
        args["who"] = event.as<TE::RiichiCalled>().who;
        break;
    case TE::Type::RIICHI_ESTABLISHED:
        args["who"] = event.as<TE::RiichiEstablished>().who;
        break;
    case TE::Type::BARKED: {
        const auto &a = event.as<TE::Barked>();
        args["who"] = a.who;
        args["bark"] = a.bark;
        args["spin"] = a.spin;
        break;
    }
    case TE::Type::ROUND_ENDED: {
        const auto &a = event.as<TE::RoundEnded>();
        args["result"] = util::stringOf(a.result);
        args["openers"] = a.openers;
        args["gunner"] = a.gunner;
        args["forms"] = a.forms;
        break;
    }
    case TE::Type::TABLE_ENDED: {
        const auto &a = event.as<TE::TableEnded>();
        args["ranks"] = a.ranks;
        args["scores"] = a.scores;
        break;
    }
    case TE::Type::POPPED_UP:
        args["who"] = event.as<TE::PoppedUp>().who;
        break;
    default:
        break;
    }

    return env.create_with(
        "type", util::stringOf(event.type()),
        "args", args
                );
}



} // namespace saki
