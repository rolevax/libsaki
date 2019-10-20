#include "lua_class.h"
#include "lua_managed_ref.h"
#include "../table/table.h"
#include "../util/dismember.h"
#include "../util/string_enum.h"

namespace saki
{



auto isValidSuitStr(const std::string &s) -> bool
{
    return s.size() == 1 && T34::isValidSuit(s[0]);
}

auto toLower(const char *s) -> std::string
{
    std::string res(s);
    std::for_each(res.begin(), res.end(), [](char &c) {
        c = static_cast<char>(std::tolower(c));
    });
    return res;
}

auto parseMountExit(const std::string &s) -> std::pair<Mount::Exit, bool>
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

auto tableToDrids(sol::table idsTable) -> util::Stactor<T37, 5>
{
    util::Stactor<T37, 5> res;

    if (!idsTable)
        return res;

    for (const auto &[key, ref] : idsTable) {
        (void) ref; // conv-to-opt doesn't work somehow

        if (res.full())
            break;

        if (sol::optional<T34> t34 = idsTable[key]; t34)
            res.emplaceBack(t34->id34());
        else if (sol::optional<T37> t37 = idsTable[key]; t37)
            res.emplaceBack(*t37);
    }

    return res;
}

auto handMod1(sol::table ids, const LuaManagedRef<Hand> &hand) -> int
{
    return tableToDrids(std::move(ids)) % *hand.data();
}

auto handMod2(T34 indic, const LuaManagedRef<Hand> &hand) -> int
{
    return indic % *hand.data();
}

auto formCtor1(LuaUserErrorHandler &error,
               const Hand &full, const FormCtx &ctx, const Rule &rule,
               sol::table drids, sol::table urids)
    -> sol::optional<Form>
{
    if (full.step() != -1) {
        error.handleUserError("EFrmNoAgr");
        return sol::nullopt;
    }

    return Form(full, ctx, rule, tableToDrids(std::move(drids)), tableToDrids(std::move(urids)));
}

auto formCtor2(LuaUserErrorHandler &error,
               const Hand &ready, const T37 &pick, const FormCtx &ctx, const Rule &rule,
               sol::table drids, sol::table urids)
    -> sol::optional<Form>
{
    if (ready.peekPickStep(pick) != -1) { // NOLINT(cppcoreguidelines-slicing)
        error.handleUserError("EFrmNoAgr");
        return sol::nullopt;
    }

    return Form(ready, pick, ctx, rule, tableToDrids(std::move(drids)), tableToDrids(std::move(urids)));
}

auto tableGetFocus(const Table &table)
    -> std::tuple<sol::optional<Who>, sol::optional<T37>, bool>
{
    using Tuple = decltype(tableGetFocus(*static_cast<Table *>(nullptr)));
    TableFocus focus = table.getFocus();
    if (focus.who().nobody())
        return Tuple(sol::nullopt, sol::nullopt, false);

    return Tuple(focus.who(), table.getFocusTile(), focus.isDiscard());
}

void setupLuaClasses(const sol::environment &env, LuaUserErrorHandler &error)
{
    setupLuaRand(env);
    setupLuaTile(env, error);
    setupLuaWho(env);
    setupLuaMeld(env, error);
    setupLuaExist(env, error);
    setupLuaMount(env);
    setupLuaTileCount(env);
    setupLuaHand(env);
    setupLuaForm(env, error);
    setupLuaRule(env);
    setupLuaFormCtx(env);
    setupLuaGame(env);
}

void setupLuaRand(sol::environment env)
{
    using LuaRand = LuaManagedRef<util::Rand>;
    env.new_usertype<LuaRand>(
        "Rand",
        "gen", LuaRand::makeMutableFunction(+[](util::Rand &r, int mod) { return r.gen(mod); }),
        "state", LuaRand::makeConstMethod(&util::Rand::state)
    );
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
            [&error](const std::string &s) {
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
        "all", sol::var(sol::as_table(std::vector<T34>(tiles34::ALL34.begin(), tiles34::ALL34.end())))
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
            [&error](const std::string &s) {
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
        sol::meta_function::equal_to, &T37::operator==,
        sol::meta_function::less_than, &T37::operator<,
        sol::meta_function::modulus, &T37::operator%,
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
            [](Exist &exist, const T37 &t, int delta) {
                exist.incMk(t, delta);
            },
            [](Exist &exist, T34 t, int delta) {
                exist.incMk(t, delta);
            }
        )
    );
}

void setupLuaMount(sol::environment env)
{
    using LuaMount = LuaManagedRef<Mount>;
    env.new_usertype<LuaMount>(
        "Mount",
        "remainpii", LuaMount::makeConstMethod(&Mount::remainPii),
        "remainrinshan", LuaMount::makeConstMethod(&Mount::remainRinshan),
        "remaina", sol::overload(
            LuaMount::makeConstFunction(+[](const Mount &mount, const T37 &t) {
                return mount.remainA(t);
            }),
            LuaMount::makeConstFunction(+[](const Mount &mount, T34 t) {
                return mount.remainA(t);
            })
        ),
        "getdrids", LuaMount::makeConstMethodAsTable(&Mount::getDrids),
        "geturids", LuaMount::makeConstMethodAsTable(&Mount::getUrids),
        "lighta", sol::overload(
            LuaMount::makeMutableFunction(+[](Mount &mount, const T37 &t, int mk, bool rin) {
                mount.lightA(t, mk, rin);
            }),
            LuaMount::makeMutableFunction(+[](Mount &mount, T34 t, int mk, bool rin) {
                mount.lightA(t, mk, rin);
            }),
            LuaMount::makeMutableFunction(+[](Mount &mount, const T37 &t, int mk) {
                mount.lightA(t, mk);
            }),
            LuaMount::makeMutableFunction(+[](Mount &mount, T34 t, int mk) {
                mount.lightA(t, mk);
            })
        ),
        "lightb", sol::overload(
            LuaMount::makeMutableFunction(+[](Mount &mount, const T37 &t, int mk, bool rin) {
                mount.lightB(t, mk, rin);
            }),
            LuaMount::makeMutableFunction(+[](Mount &mount, T34 t, int mk, bool rin) {
                mount.lightB(t, mk, rin);
            }),
            LuaMount::makeMutableFunction(+[](Mount &mount, const T37 &t, int mk) {
                mount.lightB(t, mk);
            }),
            LuaMount::makeMutableFunction(+[](Mount &mount, T34 t, int mk) {
                mount.lightB(t, mk);
            })
        ),
        "incmk", sol::overload(
            LuaMount::makeMutableFunctionError(+[](LuaUserErrorHandler &error, Mount &mount, const std::string &exit, size_t pos, const T37 &t, int delta, bool bSpace) {
                auto [e, ok] = parseMountExit(exit);
                if (!ok) {
                    error.handleUserError("EInvMntExt");
                    return;
                }

                mount.incMk(e, pos, t, delta, bSpace);
            }),
            LuaMount::makeMutableFunctionError(+[](LuaUserErrorHandler &error, Mount &mount, const std::string &exit, size_t pos, T34 t, int delta, bool bSpace) {
                auto [e, ok] = parseMountExit(exit);
                if (!ok) {
                    error.handleUserError("EInvMntExt");
                    return;
                }

                mount.incMk(e, pos, t, delta, bSpace);
            })
        ),
        "loadb", LuaMount::makeMutableMethod(&Mount::loadB)
    );
}

void setupLuaTileCount(sol::environment env)
{
    using LuaTc = LuaManagedRef<TileCount>;
    env.new_usertype<LuaTc>(
        "Tilecount",
        "ct", sol::overload(
            LuaTc::makeConstFunction(+[](const TileCount &tc, const T37 &t) {
                return tc.ct(t);
            }),
            LuaTc::makeConstFunction(+[](const TileCount &tc, T34 t) {
                return tc.ct(t);
            }),
            LuaTc::makeConstFunctionError(+[](LuaUserErrorHandler &error, const TileCount &tc, std::string suit) {
                if (!isValidSuitStr(suit)) {
                    error.handleUserError("EInvSuit");
                    return 0;
                }

                return tc.ct(T34::suitOf(suit[0]));
            })
        )
    );
}

void setupLuaHand(sol::environment env)
{
    using LuaHand = LuaManagedRef<Hand>;
    env.new_usertype<LuaHand>(
        "Hand",
        sol::meta_function::construct, +[](const LuaHand &hand) {
            return LuaHand(new Hand(*hand.data()), hand.error(), true, false);
        },
        "closed", LuaHand::makeConstMethodAsConstRef(&Hand::closed),
        "ct", LuaHand::makeConstMethod(&Hand::ct),
        "ctaka5", LuaHand::makeConstMethod(&Hand::ctAka5),
        "ready", LuaHand::makeConstMethod(&Hand::ready),
        "step", LuaHand::makeConstMethod(&Hand::step),
        "step4", LuaHand::makeConstMethod(&Hand::step4),
        "step7", LuaHand::makeConstMethod(&Hand::step7),
        "step7gb", LuaHand::makeConstMethod(&Hand::step7Gb),
        "step13", LuaHand::makeConstMethod(&Hand::step13),
        "effa", LuaHand::makeConstMethodAsTable(&Hand::effA),
        "effa4", LuaHand::makeConstMethodAsTable(&Hand::effA4),
        "ismenzen", LuaHand::makeConstMethod(&Hand::isMenzen),
        "barks", LuaHand::makeConstMethodAsTable(&Hand::barks),
        "canchii", LuaHand::makeConstMethod(&Hand::canChii),
        "canchiiasleft", LuaHand::makeConstMethod(&Hand::canChiiAsLeft),
        "canchiiasmiddle", LuaHand::makeConstMethod(&Hand::canChiiAsMiddle),
        "canchiiasright", LuaHand::makeConstMethod(&Hand::canChiiAsRight),
        "canpon", LuaHand::makeConstMethod(&Hand::canPon),
        "candaiminkan", LuaHand::makeConstMethod(&Hand::canDaiminkan),
        sol::meta_function::modulus, sol::overload(handMod1, handMod2),
        "draw", LuaHand::makeMutableFunctionError(+[](LuaUserErrorHandler &error, Hand &hand, const T37 &in) {
            if (hand.hasDrawn()) {
                error.handleUserError("EDrmCntDrw");
                return;
            }

            hand.draw(in);
        }),
        "swapout", LuaHand::makeMutableFunctionError(+[](LuaUserErrorHandler &error, Hand &hand, const T37 &out) {
            if (!(hand.closed().ct(out) > 0) && hand.hasDrawn()) {
                error.handleUserError("EDrmCntSwp");
                return;
            }

            hand.swapOut(out);
        }),
        "spinout", LuaHand::makeMutableFunctionError(+[](LuaUserErrorHandler &error, Hand &hand) {
            if (!hand.hasDrawn()) {
                error.handleUserError("EDrmCntSwp");
                return;
            }

            hand.spinOut();
        })
    );
}

void setupLuaForm(sol::environment env, LuaUserErrorHandler &error)
{
    using namespace std::placeholders;

    env.new_usertype<Form>(
        "Form",
        sol::meta_function::construct, sol::factories(
            [&error](const Hand &full, const FormCtx &ctx, const Rule &rule,
                     sol::table drids, sol::table urids) {
                return formCtor1(error, full, ctx, rule, std::move(drids), std::move(urids));
            },
            [&error](const Hand &full, const FormCtx &ctx, const Rule &rule,
                     sol::table drids) {
                return formCtor1(error, full, ctx, rule, std::move(drids), sol::nil);
            },
            [&error](const Hand &full, const FormCtx &ctx, const Rule &rule) {
                return formCtor1(error, full, ctx, rule, sol::nil, sol::nil);
            },
            [&error](const Hand &ready, const T37 &pick, const FormCtx &ctx, const Rule &rule,
                     sol::table drids, sol::table urids) {
                return formCtor2(error, ready, pick, ctx, rule, std::move(drids), std::move(urids));
            },
            [&error](const Hand &ready, const T37 &pick, const FormCtx &ctx, const Rule &rule,
                     sol::table drids) {
                return formCtor2(error, ready, pick, ctx, rule, std::move(drids), sol::nil);
            },
            [&error](const Hand &ready, const T37 &pick,const FormCtx &ctx, const Rule &rule) {
                return formCtor2(error, ready, pick, ctx, rule, sol::nil, sol::nil);
            }
        ),
        "isprototypalyakuman", &Form::isPrototypalYakuman,
        "fu", &Form::fu,
        "han", &Form::han,
        "base", &Form::base,
        "dora", &Form::dora,
        "uradora", &Form::uradora,
        "akadora", &Form::akadora,
        "yakus", [env](const Form &f) mutable {
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
    using LuaGame = LuaManagedRef<Table>;
    env.new_usertype<LuaGame>(
        "Game",
        "gethand", LuaGame::makeConstMethodAsConstRef(&Table::getHand),
        "getround", LuaGame::makeConstMethod(&Table::getRound),
        "getextraround", LuaGame::makeConstMethod(&Table::getExtraRound),
        "getdealer", LuaGame::makeConstMethod(&Table::getDealer),
        "getselfwind", LuaGame::makeConstMethod(&Table::getSelfWind),
        "getroundwind", LuaGame::makeConstMethod(&Table::getRoundWind),
        "getmount", LuaGame::makeConstMethodAsConstRef(&Table::getMount),
        "getriver", LuaGame::makeConstMethodAsTable(&Table::getRiver),
        "getrule", LuaGame::makeConstFunction(+[](const Table &t) -> Rule { return t.getRule(); }), // return copy
        "getformctx", LuaGame::makeConstMethod(&Table::getFormCtx),
        "getfocus", LuaGame::makeConstFunction(tableGetFocus),
        "riichiestablished", LuaGame::makeConstMethod(&Table::riichiEstablished)
    );
}

auto toLuaTable(sol::environment env, const TableEvent &event) -> sol::table
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
        args["openers"] = sol::as_table(a.openers);
        args["gunner"] = a.gunner;
        args["forms"] = sol::as_table(a.forms);
        break;
    }
    case TE::Type::TABLE_ENDED: {
        const auto &a = event.as<TE::TableEnded>();
        args["ranks"] = sol::as_table(a.ranks);
        args["scores"] = sol::as_table(a.scores);
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
