#ifndef TT_I
#define TT_I

#include <optional>

template<class TT_Entry,class Key>
class ITranspositionTable
{
public:
    virtual ~ITranspositionTable() {};
    virtual void put(TT_Entry &t, Key &__k) = 0;
    virtual std::optional<TT_Entry> get(const Key &__k) = 0;
    virtual void clear() = 0;
    virtual void cacheHit() = 0;
    virtual void cachePut() = 0;
    virtual int cacheTotalAccess() const = 0;
    virtual void resetStat() = 0;
};

#endif