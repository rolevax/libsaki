#include "myrand.h"

#include <random>
#include <chrono>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <string>

static std::minstd_rand generator;
static std::uniform_int_distribution<int> distribution(0, 2147483647);

uint32_t mySrand()
{
    uint32_t s = std::chrono::system_clock::now().time_since_epoch().count();
//uint32_t s = 1744748566;
    generator.seed(s);
    return s;
}

int32_t myRand()
{
    return distribution(generator);
}

uint32_t myState()
{
    std::ostringstream oss;
    oss << generator;
    std::string str(oss.str());
    return std::strtoul(str.c_str(), nullptr, 10);
}

void mySetState(uint32_t state)
{
    std::stringstream ss;
    ss << state;
    ss >> generator;
}
