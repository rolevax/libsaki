#ifndef SAKI_TEST_H
#define SAKI_TEST_H

#include <chrono>



namespace saki
{



class TestScope
{
public:
    TestScope(const char *str, bool nl = false);
    ~TestScope();

private:
    std::chrono::steady_clock::time_point mStart;
};




void testAll();

void testUtil();
void testTileCount();
void testParse4();
void testParse7And13();
void testParseAll();
void testHand();
void testForm();
void testFormGb();
void testTable();



} // namespace saki



#endif // SAKI_TEST_H
