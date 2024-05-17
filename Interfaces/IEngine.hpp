#ifndef I_ENGINE
#define I_ENGINE

#include "../StopWatch.hpp"

#include <vector>

// Forward Declaration
class StopWatch;

namespace AI
{
    namespace Interface
    {
        /**
         * @brief Engine Interface Typed parameters
         * 
         * @tparam G 
         */
        template <typename G>

        class IEngine
        {
        protected:
            virtual void _resetCutoffs() = 0;
            virtual void _computeSliceTimeLimit(StopWatch &__globalTimer, StopWatch &__mustSetTimer, int __remainingMoves, int __threads) = 0;

        public:
            virtual ~IEngine(){};

            // SEARCHES

            /**
             * @brief Search in the state space the solution using a sequential procedure
             *      the best result is returned at the end
             *
             * @param __startingPosition where to start searching throught the state spaces
             * @return GameState the best solution found
             */
            virtual G Search(G &__startingPosition) = 0;
            virtual G ParallelSearch(G &__startingPosition, int __threads) = 0;
            virtual G TimeLimitedSearch(G &__startingPosition, StopWatch &_globalTimer, int __threads) = 0;

            // Q SEARCH

            /**
             * @brief Quiescence Search Algorithm, addinitional deeper search when reaching leafs in state space trees
             * usually search only the "non quiesce" moves ( kills or game ends )
             *
             * @param __currentMove where to start the q search
             * @param __qDepth how deep we are in the trees
             * @param __alpha alpha value
             * @param __beta beta value
             * @param __color current game state color
             * @return int
             */
            virtual int Quiesce(G &__currentMove, int __qDepth, int __alpha, int __beta, int __color) = 0;
            virtual int QuiesceTimeLimited(G &__currentMove, int __qDepth, int __alpha, int __beta, int __color) = 0;

            // KILLER MOVES

            virtual void getMoves(G &__move, std::vector<G> &__moves) = 0;
            virtual void storeKillerMove(G &__t, int __depth) = 0;

            // USED IN SEARCH ALGORITHM
            virtual void sortMoves(std::vector<G> &__moves, int __depth, bool __color) = 0;
            virtual int evaluate(G &__move, int __depth, bool __color) = 0;
            virtual void addHashToMoves(std::vector<G> &__moves) = 0;

            // UTILITIES
            virtual void print() = 0;
            virtual void printHeuristic() = 0;
            virtual int getBestScore() const = 0;
            virtual int getTotalMoves() const = 0;
            virtual int getQTotalMoves() const = 0;
            virtual int getCutOffs(int __index) const = 0;
            virtual void resetTranspositionTable() = 0;

            virtual void reset() = 0;
            virtual void resetStats() = 0;
        };

    }
}
#endif