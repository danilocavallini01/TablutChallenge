#include "TranspositionTable.h"

TranspositionTable::TranspositionTable()
{
    map = std::unordered_map<ZobristKey, Tablut>();
}

TranspositionTable::~TranspositionTable() {}

void TranspositionTable::put(Tablut &t, ZobristKey k)
{
    map.insert({k, t});
}

std::optional<Tablut> TranspositionTable::get(ZobristKey k)
{
    std::unordered_map<ZobristKey, Tablut>::iterator res = map.find(k);
    if (res == map.end())
    {
        return {};
    }

    return res->second;
}