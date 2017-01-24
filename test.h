#ifndef TEST_H
#define TEST_H

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
void testHand();
void testForm();
void testTable();



} // namespace saki



#endif // TEST_H


