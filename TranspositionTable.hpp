#ifndef TRANSPOSITION_TABLE
#define TRANSPOSITION_TABLE

#include "Zobrist.hpp"
#include "Interfaces/ITranspositionTable.hpp"

#include <unordered_map>
#include <optional>
#include <mutex>
#include <iostream>

#include <boost/unordered/concurrent_flat_map.hpp>

// Forward definition
class Zobrist;

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
    COLOR_INDEX = 3,
};

/* Entry node of transposition table defined as follow:
    <SCORE,DEPTH,TT_FLAG>
*/

typedef std::tuple<int, int, FLAG, bool> Entry;


class TranspositionTable : public ITranspositionTable<Entry, ZobristKey>
{
private:
    boost::unordered::concurrent_flat_map<ZobristKey, Entry> _map;
    unsigned int _cacheHit{0};
    unsigned int _cachePut{0};

public:
    TranspositionTable() : _cacheHit(0), _cachePut(0)
    {
        _map = boost::unordered::concurrent_flat_map<ZobristKey, Entry>();
    }

    ~TranspositionTable() {}

    friend std::ostream &operator<<(std::ostream &out, const TranspositionTable &p)
    {
        out << "PERFORMANCE TT-> HITS: " << p._cacheHit << " ,PUTS: " << p._cachePut << ", TOTAL: " << p.cacheTotalAccess();
        return out;
    }

    void put(Entry &t, ZobristKey &__k)
    {
        _map.try_emplace(__k, t);
    }

    // Get entry by giving the paired key
    std::optional<Entry> get(const ZobristKey &__k)
    {
        std::optional<Entry> entry;
        _map.visit(__k, [&](const auto &__entry)
                   { entry = __entry.second; });
        return entry;
    }

    void clear()
    {
        _map.clear();
        resetStat();
    }

    void cacheHit()
    {
        _cacheHit++;
    }

    void cachePut()
    {
        _cachePut++;
    }

    int cacheTotalAccess() const
    {
        return _cachePut + _cacheHit;
    }

    void resetStat()
    {
        _cacheHit = 0;
        _cachePut = 0;
    }
};

#endif
