#ifndef PRINCESS_H
#define PRINCESS_H

#include "table.h"

#include <array>
#include <memory>



namespace saki
{



class Princess
{
public:
    explicit Princess(const Table &table, Mount &mount,
                      const std::array<std::unique_ptr<Girl>, 4> &girls);

    Princess(const Princess &copy) = delete;
    Princess &operator=(const Princess &assign) = delete;

    std::array<Hand, 4> deal();

    // the number '4' is hard-coded everywhere within this class
    // it is also assumed within this class that the four enums
    // is defined in exactly this order such that raw ints are used
    // and casted everywhere.
    // be cautious if this enum is modified.
    enum class Indic { DORA, URADORA, KANDORA, KANURA };

    bool imagedAsDora(T34 t, Indic which) const;
    bool mayHaveDora(T34 t) const;
    bool hasImageIndic(Indic which) const;
    T34 getImageIndic(Indic which) const;

    const Table &getTable() const;

private:
    std::array<TileCount, 4> nonMonkey();
    std::array<Hand, 4> monkey(std::array<TileCount, 4> &nonMonkeys);
    void doraMatters();
    T34 pickIndicator(const std::array<bool, 34> &exceptId34s, bool wall);
    void fixIndicator(Indic which, const std::array<bool, 34> &exceptId34s, bool wall);

private:
    const Table &mTable;
    Mount &mMount;
    const std::array<std::unique_ptr<Girl>, 4> &mGirls; // const array but modifying pointer

    std::array<bool, 4> mHasImageIndics;
    std::array<T34, 4> mImageIndics;
};



} // namespace saki



#endif // PRINCESS_H


