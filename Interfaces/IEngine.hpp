#ifndef I_ENGINE
#define I_ENGINE

#include "../StopWatch.hpp"

#include <vector>

// Forward Declaration
class StopWatch;

template <typename GameState>
class IEngine
{
protected:
    virtual void _resetCutoffs() = 0;
    virtual void _computeSliceTimeLimit(StopWatch &__globalTimer, StopWatch &__mustSetTimer, int __remainingMoves, int __threads) = 0;

public:
    virtual ~IEngine(){};

    // SEARCHES
    virtual GameState Search(GameState &__startingPosition) = 0;
    virtual GameState ParallelSearch(GameState &__startingPosition, int __threads) = 0;
    virtual GameState TimeLimitedSearch(GameState &__startingPosition, StopWatch &_globalTimer, int __threads) = 0;

    // Q SEARCH
    virtual int Quiesce(GameState &__currentMove, int __qDepth, int __alpha, int __beta, int __color) = 0;
    virtual int QuiesceTimeLimited(GameState &__currentMove, int __qDepth, int __alpha, int __beta, int __color) = 0;

   
    // KILLER MOVES
    virtual void sortMoves(std::vector<GameState> &__moves, int __depth, bool __color) = 0;
    virtual void getMoves(GameState &__move, std::vector<GameState> &__moves) = 0;
    virtual void storeKillerMove(GameState &__t, int __depth) = 0;

 // UTILITIES
    virtual int evaluate(GameState &__move, int __depth, bool __color) = 0;
    virtual void addHashToMoves(std::vector<GameState> &__moves) = 0;
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

#endif