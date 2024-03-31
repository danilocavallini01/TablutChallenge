#include "TranspositionTable.h"

std::mutex mtx_write;
std::mutex mtx_read;

TranspositionTable::TranspositionTable()
{
    map = std::unordered_map<ZobristKey, Entry>();
}

TranspositionTable::~TranspositionTable() {}

void TranspositionTable::put(Entry &t, ZobristKey k)
{
    TranspositionTable::cachePut();
    mtx_write.lock();
    map.emplace(k, t);
    mtx_write.unlock();
}

std::optional<Entry> TranspositionTable::get(const ZobristKey k)
{
    TranspositionTable::cacheHit();
    if (auto res = map.find(k); res != map.end())
    {
        return res->second;
    }
    return {};
}