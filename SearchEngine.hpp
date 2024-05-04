#ifndef SEARCHENGINE
#define SEARCHENGINE

#include "Tablut.h"
#include "Heuristic.hpp"
#include "MoveGenerator.hpp"
#include "TranspositionTable.hpp"
#include "StopWatch.hpp"
#include "Zobrist.hpp"

#include <limits>
#include <thread>
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
const int MAX_DEFAULT_DEPTH = 8;
const int MAX_DEFAULT_QSEARCH_DEPTH = 2;

// Max possible error accepted by time limited search algorithm ( EXPRESSED AS PERCENTAGE: ES. 20% = 20.0)
const float MAX_TIME_ERROR = 20.0;

// Default thread parameter
const int MAX_THREADS = int(std::thread::hardware_concurrency());

// Total moves
std::atomic<int> _totalMoves;
std::atomic<int> _qTotalMoves;

// Total cutoff made by alpha beta prunings
std::atomic<int> _cutOffs[MAX_DEFAULT_DEPTH];

class SearchEngine
{
protected:
    // Heuristic function: gives score to each Board position
    Heuristic _heuristic;

    // MoveGenerator algorithm to create all possible tablut moves and hashes for the transposition Table
    MoveGenerator _moveGenerator;

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
    explicit SearchEngine(int __maxDepth = MAX_DEFAULT_DEPTH, int __quiesceMaxDepth = MAX_DEFAULT_QSEARCH_DEPTH, bool __colored = false,
                          Heuristic __heuristic = Heuristic(), Zobrist __zobrist = Zobrist(),
                          MoveGenerator __moveGenerator = MoveGenerator(), TranspositionTable __transpositionTable = TranspositionTable())
        : _heuristic(__heuristic),
          _zobrist(__zobrist),
          _moveGenerator(__moveGenerator),
          _transpositionTable(__transpositionTable),
          _maxDepth(__maxDepth),
          _quiesceMaxDepth(-__quiesceMaxDepth),
          _colored(__colored)
    {
    }

    virtual Tablut Search(Tablut &__startingPosition);

    int evaluate(Tablut &__move, int __depth, bool __color)
    {
        return _heuristic.evaluate(__move, __depth, __color, _colored);
    }

    void addHashToMoves(std::vector<Tablut> &__move)
    {

        for (auto &nextTablut : __move)
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
        _moveGenerator.generateLegalMoves(__move, __moves);
    }

    int Quiesce(Tablut &__currentMove, int __qDepth, int __alpha, int __beta, int __color)
    {
        _qTotalMoves++;

        // QUIESCENCE MAX DEPTH OR GAME OVER CONDITION
        if (__currentMove.isGameOver() || __qDepth == _quiesceMaxDepth)
        {
            return evaluate(__currentMove, __qDepth, __color);
        }

        int score;
        std::vector<Tablut> moves;

        int standPat = evaluate(__currentMove, __qDepth, __color);

        if (standPat >= __beta)
        {
            return __beta;
        }

        __alpha = std::max(__alpha, standPat);

        // GENERATE ALL NON QUIET MOVES ( CAPTURE OR WINS )
        getMoves(__currentMove, moves);

        // NO NON-QUIET MOVE AVAILABLE, QUIET STATE SO WE RETURN NORMAL HEURISTIC VALUE
        if (moves.size() == 0)
        {
            return evaluate(__currentMove, __qDepth, __color);
        }

        for (Tablut &move : moves)
        {
            // CHECK IF MOVE IS NON QUIET
            if (move.isNonQuiet())
            {
                score = -Quiesce(move, __qDepth - 1, -__beta, -__alpha, !__color);
                if (score >= __beta)
                {
                    return __beta;
                }

                __alpha = std::max(__alpha, score);
            }
        }

        return __alpha;
    }

    friend std::ostream &operator<<(std::ostream &out, SearchEngine &__engine)
    {
        out << " --> ENGINE SCORE = " << __engine._bestScore << std::endl;

        // PERFORMANCE _______________
        out << __engine._transpositionTable << std::endl;

        out << "TOTAL MOVES CHECKED: " << __engine.getTotalMoves() << ",Q: " << __engine.getQTotalMoves() << std::endl;
        out << "TOTAL CUTOFFS ";

        for (int i = 0; i < MAX_DEFAULT_DEPTH; i++)
        {
            out << __engine.getCutOffs(i) << ",";
        }

        out << std::endl;
        return out;
    }

    void print()
    {
        std::cout << this << std::endl;
    }

    void printHeuristic()
    {
        _heuristic.print();
    }

    int getBestScore()
    {
        return _bestScore;
    }

    int getTotalMoves()
    {
        return _totalMoves;
    }

    int getQTotalMoves()
    {
        return _qTotalMoves;
    }

    int getCutOffs(int index)
    {
        return _cutOffs[index];
    }

    void resetTranspositionTable()
    {
        _transpositionTable.clear();
    }

    void resetStats()
    {
        _totalMoves = 0;
        _qTotalMoves = 0;
        _resetCutoffs();
    }
};

#endif