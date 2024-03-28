#ifndef TRANSPOSITION_TABLE
#define TRANSPOSITION_TABLE

#include <Tablut.h>
#include <Zobrist.h>
#include <unordered_map>
#include <optional>

class TranspositionTable
{
private:
    std::unordered_map<ZobristKey,Tablut> map;
public:
    TranspositionTable();
    ~TranspositionTable();

    void put(Tablut &t, ZobristKey k);
    std::optional<Tablut> get(ZobristKey k);
};

#endif