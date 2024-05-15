#ifndef MOVE_GENERATOR
#define MOVE_GENERATOR

#include <vector>

namespace AI
{
    namespace Interface
    {
        template <class GameState>
        class IMoveGenerator
        {
        public:
            virtual ~IMoveGenerator(){};
            static void generateLegalMoves(GameState &__game, std::vector<GameState> &__nexts) {};
        };
    }
}

#endif