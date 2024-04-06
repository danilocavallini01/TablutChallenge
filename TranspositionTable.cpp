#include "TranspositionTable.h"

std::mutex mtxCacheHit;
std::mutex mtxCachePut;

TranspositionTable::TranspositionTable()
{
    _map = boost::unordered::concurrent_flat_map<ZobristKey, Entry>();
}

TranspositionTable::~TranspositionTable() {}

void TranspositionTable::put(Entry &t, ZobristKey &__k)
{
    _map.try_emplace(__k, t);
}

std::optional<Entry> TranspositionTable::get(const ZobristKey &__k)
{
    std::optional<Entry> entry;
    _map.visit(__k, [&entry](const auto &__entry)
               { entry = __entry.second; });
    return entry;
}

void TranspositionTable::cacheHit()
{
    _cacheHit++;
}

void TranspositionTable::cachePut()
{
    _cachePut++;
}