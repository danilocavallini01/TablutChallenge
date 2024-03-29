#include "TranspositionTable.h"

TranspositionTable::TranspositionTable()
{
    map = std::unordered_map<ZobristKey, Entry>();
}

TranspositionTable::~TranspositionTable() {}

void TranspositionTable::put(Entry &t, ZobristKey k)
{
    map.insert({k, t});
}

std::optional<Entry> TranspositionTable::get(ZobristKey k)
{
    std::unordered_map<ZobristKey, Entry>::iterator res = map.find(k);
    if (res == map.end())
    {
        return {};
    }

    return res->second;
}