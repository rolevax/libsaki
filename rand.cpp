#include "rand.h"

#include <chrono>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <string>



namespace saki
{



Rand::Rand()
    : mDist(0, 2147483647)
{
    uint32_t s = std::chrono::system_clock::now().time_since_epoch().count();
    mGen.seed(s);
}

int32_t Rand::gen()
{
    return mDist(mGen);
}

int32_t Rand::gen(int32_t mod)
{
    return gen() % mod;
}

uint32_t Rand::state() const
{
    std::ostringstream oss;
    oss << mGen;
    std::string str(oss.str());
    return std::strtoul(str.c_str(), nullptr, 10);
}

void Rand::set(uint32_t state)
{
    std::stringstream ss;
    ss << state;
    ss >> mGen;
}



} // namespace saki


