#ifndef I_HEURISTIC
#define I_HEURISTIC

#include <vector>

namespace AI
{
    namespace Define
    {
        typedef std::tuple<int, int, int, int> StandardMove;
    }

    namespace Interface
    {

        template <typename GameState, typename Move = AI::Define::StandardMove>
        class IHeuristic
        {
        protected:
            virtual void _killerMoveHit() = 0;

        public:
            virtual ~IHeuristic(){};

            // EVALUATE POSITION
            virtual int evaluate(GameState &__game, int __depth, bool __color, bool __colored) = 0;
            virtual int quickEvaluate(GameState &__game, int __depth, bool __color, bool __colored) = 0;

            // KILLER MOVES
            virtual void storeKillerMove(Move &__kMove, int __depth) = 0;
            virtual int isKillerMove(Move &__kMove, int __depth) = 0;
            virtual int computeKillerMovesScore(GameState &__game, int __depth, bool __color) = 0;
            virtual int getKillerMovesHits() const = 0;
            virtual void resetKillerMoves() = 0;

            // MOVE ORDERING
            virtual bool compare(GameState &__game1, GameState &__game2) = 0;
            virtual void sortMoves(std::vector<GameState> &__moves, int __depth, bool __whiteEvaluate, bool __colored) = 0;

            // UTILITIES
            virtual void print() = 0;
        };
    }
}

#endif