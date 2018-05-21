#include "test.h"
#include "../form/tile_count_list.h"
#include "../form/form.h"
#include "../form/form_gb.h"
#include "../table/table_tester.h"
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
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - mStart).count();
    std::cout << " PASS " << t << "ms" << std::endl;
}



void testAll()
{
    // *INDENT-OFF*
//    testUtil();
//    testTileCount();
//    testParse4();
//    testParse7And13();
    testParseAll();
//    testHand();
//    testForm();
//    testFormGb();
//    testTable();
    // *INDENT-ON*
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

void testParse4()
{
}

void testParse7And13()
{
    TestScope test("parse 7/13", true);

    using namespace tiles37;

    TileCountList tcl(13, 1_p, 9_p);
    int prevPercent = 0;
    for (const TileCount &tc : tcl) {
        int currProg = tc.ct(1_p) * 16 + tc.ct(2_p) * 4 + tc.ct(3_p);
        int percent = (currProg * 100) / 128;
        if (percent != prevPercent) {
            util::p(percent, "%");
            prevPercent = percent;
        }

        auto parsed = tc.parse7();
        int step = parsed.step7();

        if (step != tc.step7()) {
            util::p(tc.t37s13(true));
            util::p("old", tc.step7());
            util::p("new", step);
            std::abort();
        }
    }
}

void testParseAll()
{
    TestScope test("parse all", true);

    using namespace tiles37;

    TileCountList tcl(13, 1_p, 9_p);
    int prevPercent = 0;
    for (const TileCount &tc : tcl) {
        int currProg = tc.ct(1_p) * 16 + tc.ct(2_p) * 4 + tc.ct(3_p);
        int percent = (currProg * 100) / 128;
        if (percent != prevPercent) {
            util::p(percent, "%");
            prevPercent = percent;
        }

        int step = tc.parse(0).step();
        int old = tc.step(0);
        if (step != old) {
            util::p(tc.t37s13(true));
            util::p("old", old);
            util::p("new", step);
            std::abort();
        }
    }
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
    TileCount close { 1_m, 2_m, 3_m, 2_p, 3_p, 4_p, 3_s, 4_s, 5_s, 4_s, 5_s, 7_s, 7_s };
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
    std::array<TableDecider *, 4> deciders;
    std::vector<TableObserver *> obs;
    Rule rule;
    for (int iter = 0; iter < 20; iter++) {
        util::p(iter);

        std::array<std::unique_ptr<Girl>, 4> girls;
        for (int w = 0; w < 4; w++) {
            assert(girlIds[w] != 1);
            Girl::Id id = Girl::Id(girlIds[w]);

            girls[w] = Girl::create(Who(w), id);
            ais[w] = Ai::create(id);
            deciders[w] = ais[w].get();
        }

        TableEnvStub env;
        Table::InitConfig config { points, std::move(girls), rule, Who(0) };
        Table table(std::move(config), obs, env);
        TableTester tester(table, deciders);
        tester.run();
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
        TileCount close1 { 1_m, 9_m, 1_p, 9_p, 1_s, 9_s, 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y };
        Hand hand1(close1);
        hand1.draw(1_m);
        FormGb form1(hand1, ctx, false);
        assert(form1.fan() == 89);
    }

    {
        TileCount close2 { 1_s, 2_s, 3_s, 3_s, 4_s, 5_s, 5_s, 6_s, 7_s, 7_s, 8_s, 9_s, 9_s };
        Hand hand2(close2);
        hand2.draw(9_s);
        FormGb form2(hand2, ctx, false);
        for (Fan f : form2.fans())
            util::p("f", util::stringOf(f));

        util::p("form2.fan", form2.fan());
    }
}



} // namespace saki
