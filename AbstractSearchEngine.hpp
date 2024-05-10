#ifndef ABSTRACT_SEARCH_ENGINE
#define ABSTRACT_SEARCH_ENGINE

#include "Tablut.h"
#include "Heuristic.hpp"
#include "MoveGenerator.hpp"
#include "TranspositionTable.hpp"
#include "StopWatch.hpp"
#include "Zobrist.hpp"

#include <thread>
#include <iostream>
#include <cstring>
#include <future>
#include <atomic>

// Forward Declaration
class Tablut;
class Heuristic;
class MoveGenerator;
class TranspositionTable;
class StopWatch;
class Zobrist;

// Lowest and Highest scores for alpha and beta initialization
const int BOTTOM_SCORE(-10000000);
const int TOP_SCORE(10000000);

// Default depth mainly used for time limited algorithm
const int MAX_DEFAULT_DEPTH = 7;
const int MAX_DEFAULT_QSEARCH_DEPTH = 3;

// Max possible error accepted by time limited search algorithm ( EXPRESSED AS PERCENTAGE: ES. 20% = 20.0)
const float MAX_TIME_ERROR = 20.0;

// Default thread parameter
const int MAX_THREADS = int(std::thread::hardware_concurrency());

// Total moves
std::atomic<int> _totalMoves;
std::atomic<int> _qTotalMoves;

// Total cutoff made by alpha beta prunings
std::atomic<int> _cutOffs[MAX_DEFAULT_DEPTH];

class AbstractSearchEngine
{
protected:
    // Heuristic function: gives score to each Board position
    Heuristic _heuristic;

    // Transposition table, used to store previous seen _board positions so we dont re-evaluate
    TranspositionTable _transpositionTable;

    // Timer for time limited search
    StopWatch _stopWatch;
    StopWatch _globalRemainingTime;

    // To Check Board and hash current game boards
    Zobrist _zobrist;

    // _maxDepth set by the last method that invoked a XXSearch() Function
    int _maxDepth;

    // Quiesce function max depth, stored as a negative value cause its used to go down the tree when
    // a leaf node is reached so u reach depth -1 -2 ...
    int _quiesceMaxDepth;

    // max score set by the last method that invoked a XXSearch() Function
    int _bestScore;

    // best move found by last method that invoked a XXSearch() Function
    Tablut _bestMove;

    // should heuristic function return a score based on the current player perspective
    bool _colored;

    void _resetCutoffs()
    {
        for (int i = 0; i < MAX_DEFAULT_DEPTH; i++)
        {
            _cutOffs[i] = 0;
        }
    }

    void _computeSliceTimeLimit(StopWatch &__globalTimer, StopWatch &__mustSetTimer, int __remainingMoves, int __threads)
    {
        int slicedTimeLimit = int(float(__globalTimer.getRemainingTime()) / std::ceil(float(__remainingMoves) / float(__threads)) * (100.0 - MAX_TIME_ERROR) / 100.0);
        std::cout << "TIME LIMIT SLICED -> TR: " << __globalTimer.getRemainingTime() << ", REMAINING MOVES:" << __remainingMoves << ", TOTAL THREAD: " << __threads << ", SLICED TIME: " << slicedTimeLimit << std::endl;
        __mustSetTimer.setTimeLimit(slicedTimeLimit);
    }

public:
    explicit AbstractSearchEngine(int __maxDepth = MAX_DEFAULT_DEPTH, int __quiesceMaxDepth = MAX_DEFAULT_QSEARCH_DEPTH, bool __colored = false,
                                  Heuristic __heuristic = Heuristic(), Zobrist __zobrist = Zobrist(), TranspositionTable __transpositionTable = TranspositionTable())
        : _heuristic(__heuristic),
          _zobrist(__zobrist),
          _transpositionTable(__transpositionTable),
          _maxDepth(__maxDepth),
          _quiesceMaxDepth(-__quiesceMaxDepth),
          _colored(__colored)
    {
    }

    virtual ~AbstractSearchEngine(){};

    virtual Tablut Search(Tablut &__startingPosition) = 0;
    virtual Tablut ParallelSearch(Tablut &__startingPosition, int __threads = MAX_THREADS) = 0;
    virtual Tablut TimeLimitedSearch(Tablut &__startingPosition, StopWatch &_globalTimer, int __threads = MAX_THREADS) = 0;

    virtual int Quiesce(Tablut &__currentMove, int __qDepth, int __alpha, int __beta, int __color) = 0;

    int evaluate(Tablut &__move, int __depth, bool __color)
    {
        return _heuristic.evaluate(__move, __depth, __color, _colored);
    }

    void addHashToMoves(std::vector<Tablut> &__moves)
    {

        for (auto &nextTablut : __moves)
        {
            _zobrist.addHash(nextTablut);
        }
    }

    void sortMoves(std::vector<Tablut> &__moves, int __depth, bool __color)
    {
        _heuristic.sortMoves(__moves, __depth, __color, _colored);
    }

    void getMoves(Tablut &__move, std::vector<Tablut> &__moves)
    {
        MoveGenerator::generateLegalMoves(__move, __moves);
    }

    void storeKillerMove(Tablut &__t, int __depth)
    {
        KillerMove move = __t.getMove();
        _heuristic.storeKillerMove(move, __depth);
    }

    friend std::ostream &operator<<(std::ostream &out, const AbstractSearchEngine &__engine)
    {
        out << " --> ENGINE SCORE = " << __engine._bestScore << std::endl;

        // PERFORMANCE _______________
        out << __engine._transpositionTable << std::endl;

        out << "TOTAL MOVES CHECKED: " << __engine.getTotalMoves() << ",Q: " << __engine.getQTotalMoves() << std::endl;
        out << "TOTAL CUTOFFS ";

        for (int i = 1; i < __engine._maxDepth; i++)
        {
            out << "D[" << i << "]:" << __engine.getCutOffs(i) << ",";
        }

        out << std::endl;

        out << "TOTAL KILLER MOVES HIT: " << __engine._heuristic.getKillerMovesHits() << std::endl;

        return out;
    }

    void print()
    {
        std::cout << *this << std::endl;
    }

    void printHeuristic()
    {
        _heuristic.print();
    }

    int getBestScore() const
    {
        return _bestScore;
    }

    int getTotalMoves() const
    {
        return _totalMoves;
    }

    int getQTotalMoves() const
    {
        return _qTotalMoves;
    }

    int getCutOffs(int index) const
    {
        return _cutOffs[index];
    }

    void resetTranspositionTable()
    {
        _transpositionTable.clear();
    }

    void reset()
    {
        resetStats();
        resetTranspositionTable();
        _heuristic.resetKillerMoves();
    }

    void resetStats()
    {
        _totalMoves = 0;
        _qTotalMoves = 0;
        _resetCutoffs();
    }
};

#endif