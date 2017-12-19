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
void testParse();
void testHand();
void testForm();
void testFormGb();
void testTable();



} // namespace saki



#endif // SAKI_TEST_H
