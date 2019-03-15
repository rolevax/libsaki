#include "rand.h"

#include <chrono>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <string>



namespace saki
{



namespace util
{



Rand::Rand()
    : mDist(0, 2147483647)
{
    auto s = std::chrono::system_clock::now().time_since_epoch().count();
    auto s32 = static_cast<uint32_t>(s);
    mGen.seed(s32);
}

int32_t Rand::gen()
{
    return mDist(mGen);
}

int32_t Rand::gen(int32_t mod)
{
    return gen() % mod;
}

uint32_t Rand::uGen(size_t mod)
{
    return static_cast<uint32_t>(gen(static_cast<int32_t>(mod)));
}

uint32_t Rand::state() const
{
    std::ostringstream oss;
    oss << mGen;
    std::string str(oss.str());
    auto s = std::strtoul(str.c_str(), nullptr, 10);
    return static_cast<uint32_t>(s);
}

void Rand::set(uint32_t state)
{
    std::stringstream ss;
    ss << state;
    ss >> mGen;
}

///
/// \brief Get reference to the Uniform Random Bit Generator
///
std::minstd_rand &Rand::getUrbg()
{
    return mGen;
}



} // namespace util



} // namespace saki
