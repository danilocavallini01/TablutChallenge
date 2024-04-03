#include "TranspositionTable.h"

TranspositionTable::TranspositionTable()
{
    map = boost::unordered::concurrent_flat_map<ZobristKey, Entry>();
}

TranspositionTable::~TranspositionTable() {}

void TranspositionTable::put(Entry &t, ZobristKey k)
{
    _cachePut++;
    map.try_emplace(k, t);
}

std::optional<Entry> TranspositionTable::get(const ZobristKey k)
{
    _cacheHit++;
    map.visit(k, [](auto &x){
        return x.second; 
    });

    return {};
}