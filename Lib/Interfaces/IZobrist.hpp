#ifndef ZOBRIST_H_I
#define ZOBRIST_H_I

#include <cstdint>

namespace AI
{
    namespace Define
    {
        typedef uint64_t ZobristKey;
    }
    namespace Interface
    {
        template <class GameState, class Key = AI::Define::ZobristKey>
        class IZobrist
        {
        public:
            virtual ~IZobrist() {}
            virtual Key hash(const GameState &__game, bool __colored) const = 0;
            virtual void addHash(GameState &__t) = 0;
        };
    }
}

#endif