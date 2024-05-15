#ifndef TT_I
#define TT_I

#include <optional>

namespace AI
{
    namespace Interface
    {
        template <class TT_Entry, class Key = AI::Define::ZobristKey>
        class ITranspositionTable
        {
        public:
            virtual ~ITranspositionTable(){};

            // CRUD TT OPERATION
            virtual void put(TT_Entry &t, Key &__k) = 0;
            virtual std::optional<TT_Entry> get(const Key &__k) = 0;

            // CLEAR TABLE
            virtual void clear() = 0;

            // TT STAT OPERATION
            virtual void cacheHit() = 0;
            virtual void cachePut() = 0;
            virtual int cacheTotalAccess() const = 0;
            virtual void resetStat() = 0;
        };
    }
}

#endif