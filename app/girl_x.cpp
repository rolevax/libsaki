#include "girl_x.h"

#include <unordered_map>



namespace saki
{



using CloneCache = std::unordered_map<const void *, kaguya::LuaRef>;

void cloneLuaTable(kaguya::LuaTable lhs, kaguya::LuaTable rhs, CloneCache &cache)
{
    for (std::string key : rhs.keys()) {
        if (rhs[key].type() == LUA_TTABLE) {
            kaguya::LuaRef ref = rhs[key];
            const void *ptrLeftVal = ref.native_pointer();
            auto it = cache.find(ptrLeftVal);
            if (it == cache.end()) {
                // creat new node
                lhs[key] = kaguya::NewTable();
                cache[ptrLeftVal] = lhs[key];

                // deep copy
                cloneLuaTable(lhs[key], rhs[key], cache);
            } else {
                // link existing node
                kaguya::LuaRef ref = it->second;
                lhs[key] = ref;
            }
        } else {
            // non-table types
            lhs[key] = rhs[key];
        }
    }
}

///
/// \brief Deep-copy a Lua table
///
/// Note that Lua table is a graph, not tree
/// We use DFS with cycle handling
///
void cloneLuaTable(kaguya::LuaTable lhs, kaguya::LuaTable rhs)
{
    CloneCache cache;
    cloneLuaTable(lhs, rhs, cache);
}



GirlX::GirlX(Who who, std::string luaCode)
    : Girl(who, Girl::Id::CUSTOM)
{
    mLua.dostring(luaCode);
}

GirlX::GirlX(const GirlX &copy)
    : Girl(copy)
{
    // the kaguya lib seems missing a const getter of global table
    kaguya::LuaTable src = const_cast<GirlX &>(copy).mLua.globalTable();

    // assume that cloning the global table is just cloning the Lua VM
    cloneLuaTable(mLua.globalTable(), src);
}

std::unique_ptr<Girl> GirlX::clone() const
{
    return std::make_unique<GirlX>(*this);
}

void GirlX::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;
    (void) mount;
    (void) rinshan;

    if (who != mSelf)
        return;

    int fuck = mLua["fuck"];
    util::p("get fuck", fuck);
}



} // namespace saki
