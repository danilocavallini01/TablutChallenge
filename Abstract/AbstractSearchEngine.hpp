#ifndef ABSTRACT_SEARCH_ENGINE
#define ABSTRACT_SEARCH_ENGINE

#include "../Interfaces/IEngine.hpp"
#include "../Heuristic.hpp"
#include "../MoveGenerator.hpp"
#include "../TranspositionTable.hpp"
#include "../Zobrist.hpp"

#include <thread>
#include <iostream>
#include <cstring>
#include <future>
#include <atomic>

// Forward Declaration
class GameState;
class Heuristic;
class MoveGenerator;
class TranspositionTable;
class Zobrist;

// Lowest and Highest scores for alpha and beta initialization
const int BOTTOM_SCORE(-10000000);
const int TOP_SCORE(10000000);

// Default depth mainly used for time limited algorithm
const int MAX_DEFAULT_DEPTH = 10;
const int MAX_DEFAULT_QSEARCH_DEPTH = 2;

// Max possible error accepted by time limited search algorithm ( EXPRESSED AS PERCENTAGE: ES. 20% = 20.0)
const float MAX_TIME_ERROR = 12.0;

// Default thread parameter
const int MAX_THREADS = int(std::thread::hardware_concurrency());

// Total moves
std::atomic<int> _totalMoves;
std::atomic<int> _qTotalMoves;

// Total cutoff made by alpha beta prunings
std::atomic<int> _cutOffs[MAX_DEFAULT_DEPTH];

template<typename GameState>
class AbstractSearchEngine : public IEngine<GameState>
{
protected:
    // Heuristic function: gives score to each std::array<std::array<C, DIM>, DIM> position
    Heuristic _heuristic;

    // Transposition table, used to store previous seen _board positions so we dont re-evaluate
    TranspositionTable _transpositionTable;

    // Timer for time limited search
    StopWatch _stopWatch;
    StopWatch _globalRemainingTime;

    // To Check std::array<std::array<C, DIM>, DIM> and hash current game boards
    Zobrist _zobrist;

    // _maxDepth set by the last method that invoked a XXSearch() Function
    int _maxDepth;

    // Quiesce function max depth, stored as a negative value cause its used to go down the tree when
    // a leaf node is reached so u reach depth -1 -2 ...
    int _quiesceMaxDepth;

    // max score set by the last method that invoked a XXSearch() Function
    int _bestScore;

    // best move found by last method that invoked a XXSearch() Function
    GameState _bestMove;

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

    virtual GameState Search(GameState &__startingPosition) = 0;
    virtual GameState ParallelSearch(GameState &__startingPosition, int __threads = MAX_THREADS) = 0;
    virtual GameState TimeLimitedSearch(GameState &__startingPosition, StopWatch &_globalTimer, int __threads = MAX_THREADS) = 0;

    virtual int Quiesce(GameState &__currentMove, int __qDepth, int __alpha, int __beta, int __color) = 0;

    int evaluate(GameState &__move, int __depth, bool __color)
    {
        return _heuristic.evaluate(__move, __depth, __color, _colored);
    }

    void addHashToMoves(std::vector<GameState> &__moves)
    {

        for (auto &nextGameState : __moves)
        {
            _zobrist.addHash(nextGameState);
        }
    }

    void sortMoves(std::vector<GameState> &__moves, int __depth, bool __color)
    {
        _heuristic.sortMoves(__moves, __depth, __color, _colored);
    }

    void getMoves(GameState &__move, std::vector<GameState> &__moves)
    {
        MoveGenerator::generateLegalMoves(__move, __moves);
    }

    void storeKillerMove(GameState &__t, int __depth)
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

    int getCutOffs(int __index) const
    {
        return _cutOffs[__index];
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