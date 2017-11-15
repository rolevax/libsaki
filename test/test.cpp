#include "test.h"
#include "../form/tile_count.h"
#include "../form/form.h"
#include "../form/form_gb.h"
#include "../table/table.h"
#include "../table/table_env_stub.h"
#include "../ai/ai.h"
#include "../util/string_enum.h"
#include "../util/misc.h"

#include <cstring>



namespace saki
{



TestScope::TestScope(const char *str, bool nl)
{
    int len = 16 - strlen(str);
    len = len > 0 ? len : 1;
    std::cout << str << std::string(len, ' ') << std::flush;
    if (nl)
        std::cout << std::endl;
    mStart = std::chrono::steady_clock::now();

}

TestScope::~TestScope()
{
    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - mStart).count();
    std::cout << " PASS " << t << "ms" << std::endl;
}



void testAll()
{
//    testUtil();
//    testTileCount();
    testParse();
//    testHand();
//    testForm();
//    testFormGb();
//    testTable();
}

void testUtil()
{
    TestScope test("util");

    std::vector<int> v { 1, 2, 3 };
    assert(!util::all(v, [](int i) { return i > 2; }));
    std::array<int, 4> a { 1, 2, 3 };
    assert(util::all(a, [](const int &i) { return i < 7; }));
}

void testTileCount()
{
    TestScope test("tile-count");

    using namespace tiles37;
    TileCount tc { 1_m, 1_m, 1_m, 2_p, 2_p, 2_p, 3_s, 3_s, 3_s, 4_f, 4_f, 4_f, 1_y, 1_y };
    assert(tc.step4(0) == -1);
    assert(tc.step(0) == -1);
}

void testParse()
{
    TestScope test("parse", true);

    using namespace tiles37;
    TileCount tc { 1_m, 2_m, 4_m, 2_p, 2_p, 3_p, 3_s, 3_s, 3_s, 4_f, 4_f, 4_f, 1_y };

    auto parseds = tc.parse4(0);

    for (const auto &p : parseds)
        util::p(p, 8 - p.work());
}

void testHand()
{
    TestScope test("hand");

    using namespace tiles37;
    TileCount tc { 1_m, 1_m, 1_m, 2_p, 2_p, 2_p, 3_s, 3_s, 3_s, 4_f, 4_f, 4_f, 1_y };
    Hand hand(tc);
    assert(hand.step() == 0);

    hand.draw(1_y);
    assert(hand.step() == -1);
}

void testForm()
{
    TestScope test("form");

    using namespace tiles37;
    TileCount close { 1_m, 2_m, 3_m, 2_p, 3_p, 4_p, 3_s, 4_s, 5_s, 4_s, 5_s,   7_s, 7_s };
    Hand hand(close);

    Rule rule;
    FormCtx ctx;
    ctx.selfWind = 1;
    ctx.roundWind = 1;

    Form form(hand, 6_s, ctx, rule);
    assert(form.hasYaku());
    assert(form.han() == 1);
    assert(form.spell() == "PnfNmi");
}

void testTable()
{
    TestScope test("table", true);

    std::array<int, 4> points { 25000, 25000, 25000, 25000 };
    std::array<int, 4> girlIds { 714915, 712715, 710113, 713314 };
    std::array<std::unique_ptr<Ai>, 4> ais;
    std::array<TableOperator*, 4> ops;
    std::vector<TableObserver*> obs;
    Rule rule;
    for (int iter = 0; iter < 20; iter++) {
        util::p(iter);
        for (int w = 0; w < 4; w++) {
            ais[w].reset(Ai::create(Who(w), Girl::Id(girlIds[w])));
            ops[w] = ais[w].get();
        }


        TableEnvStub env;
        Table table(points, girlIds, ops, obs, rule, Who(0), env);
        table.start();
    }
}

void testFormGb()
{
    TestScope test("form-gb", true);

    FormCtx ctx;
    ctx.selfWind = 2;
    ctx.roundWind = 1;

    using namespace tiles37;

    {
        TileCount close1 {1_m,9_m,1_p,9_p,1_s,9_s,1_f,2_f,3_f,4_f,1_y,2_y,3_y};
        Hand hand1(close1);
        hand1.draw(1_m);
        FormGb form1(hand1, ctx, false);
        assert(form1.fan() == 89);
    }

    {
        TileCount close2 {1_s,2_s,3_s,3_s,4_s,5_s,5_s,6_s,7_s,7_s,8_s,9_s,9_s};
        Hand hand2(close2);
        hand2.draw(9_s);
        FormGb form2(hand2, ctx, false);
        for (Fan f : form2.fans())
            util::p("f", util::stringOf(f));
        util::p("form2.fan", form2.fan());
    }
}



} // namespace saki


