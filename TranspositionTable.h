#ifndef TRANSPOSITION_TABLE
#define TRANSPOSITION_TABLE

#include "Tablut.h"
#include "Zobrist.h"
#include <unordered_map>
#include <optional>
#include <mutex>
#include <iostream>

// Entry Flag enum for values EXACT,LOWEBOUND,PPERBOUND

typedef uint8_t FLAG_CODEX;
enum FLAG : FLAG_CODEX
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
    std::unordered_map<ZobristKey, Entry> map;

public:
    TranspositionTable();
    ~TranspositionTable();

    int _cacheHit = 0;
    int _cachePut = 0;

    void put(Entry &t, ZobristKey k);
    std::optional<Entry> get(const ZobristKey k);

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

    inline void resetStat() {
        _cacheHit = 0;
        _cachePut = 0;
    }
};

#endif