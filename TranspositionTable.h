#ifndef TRANSPOSITION_TABLE
#define TRANSPOSITION_TABLE

#include <Tablut.h>
#include <Zobrist.h>
#include <unordered_map>
#include <optional>


// Entry Flag enum for values EXACT,LOWEBOUND,UPPERBOUND

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
    TABLUT_BOARD_INDEX = 3
};


/* Entry node of transposition table defined as follow:
    <SCORE,DEPTH,TT_FLAG,TABLUT_BOARD>
*/

typedef std::tuple<int, int, FLAG, Tablut> Entry;

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
    std::optional<Entry> get(ZobristKey k);

    inline void cacheHit() {
        _cacheHit++;
    }

    inline void cachePut() {
        _cachePut++;
    }

    inline int cacheMiss() {
        return _cachePut - _cacheHit;
    }
};

#endif