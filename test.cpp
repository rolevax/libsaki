#include "test.h"
#include "tilecount.h"
#include "hand.h"
#include "form.h"
#include "util.h"

#include <iostream>
#include <cassert>



using namespace saki;
using namespace saki::util;



void testAll()
{
    p("=== test start");

    testUtil();
    testTileCount();
    testHand();
    testForm();

    p("=== test done!");
}

void testUtil()
{
    p("  + util");

    std::vector<int> v { 1, 2, 3 };
    assert(!util::all(v, [](int i) { return i > 2; }));
    std::array<int, 4> a { 1, 2, 3 };
    assert(util::all(a, [](const int &i) { return i < 7; }));

    p("  -");
}

void testTileCount()
{
    p("  + tile count");
    using namespace tiles37;
    TileCount tc { 1_m, 1_m, 1_m, 2_p, 2_p, 2_p, 3_s, 3_s, 3_s, 4_f, 4_f, 4_f, 1_y, 1_y };
    assert(tc.step4(0) == -1);
    assert(tc.step(0) == -1);
    p("  -");
}

void testHand()
{
    p("  + hand");

    using namespace tiles37;
    TileCount tc { 1_m, 1_m, 1_m, 2_p, 2_p, 2_p, 3_s, 3_s, 3_s, 4_f, 4_f, 4_f, 1_y };
    Hand hand(tc);
    assert(hand.step() == 0);

    hand.draw(1_y);
    assert(hand.step() == -1);

    p("  -");
}

void testForm()
{
    p("  + form");

    using namespace tiles37;
    TileCount close { 1_m, 2_m, 3_m, 2_p, 3_p, 4_p, 3_s, 4_s, 5_s, 4_s, 5_s,   7_s, 7_s };
    Hand hand(close);

    RuleInfo rule;
    PointInfo info;
    info.selfWind = 1;
    info.roundWind = 1;

    Form form(hand, 6_s, info, rule);
    assert(form.hasYaku());
    assert(form.han() == 1);
    assert(form.spell() == "PnfNmi");

    p("  -");
}




