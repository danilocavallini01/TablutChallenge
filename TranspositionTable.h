#pragma once

#ifndef TRANSPOSITION_TABLE
#define TRANSPOSITION_TABLE

#include "Tablut.h"
#include "Zobrist.h"

#include <unordered_map>
#include <optional>
#include <mutex>
#include <iostream>

#include <boost/unordered/concurrent_flat_map.hpp>

// Entry Flag enum for values EXACT,LOWEBOUND,PPERBOUND

enum FLAG : uint8_t
{
    EXACT = 0U,
    LOWERBOUND = 1U,
    UPPERBOUND = 2U,
};

// Index for every element of the entry tuple

enum ENTRY : int
{
    SCORE_INDEX = 0,
    DEPTH_INDEX = 1,
    FLAG_INDEX = 2,
};

/* Entry node of transposition table defined as follow:
    <SCORE,DEPTH,TT_FLAG>
*/

typedef std::tuple<int, int, FLAG> Entry;

class TranspositionTable
{
private:
    boost::unordered::concurrent_flat_map<ZobristKey, Entry> _map;
    unsigned int _cacheHit{0};
    unsigned int _cachePut{0};

public:
    TranspositionTable();
    ~TranspositionTable();

    void put(Entry &__t, ZobristKey &__k);
    std::optional<Entry> get(const ZobristKey &__k);

    friend std::ostream &operator<<(std::ostream &out, const TranspositionTable &p)
    {
        out << "PERFORMANCE TT-> HITS:" << p._cacheHit << " ,PUTS:" << p._cachePut << ", TOTAL: " << p.cacheTotalAccess();
        return out;
    }

    inline void cacheHit()
    {
        _cacheHit++;
    }

    inline void cachePut()
    {
        _cachePut++;
    }

    inline int cacheTotalAccess() const
    {
        return _cachePut + _cacheHit;
    }

    inline void resetStat()
    {
        _cacheHit = 0;
        _cachePut = 0;
    }
};

#endif