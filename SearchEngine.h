#pragma once

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
const int BOTTOM_SCORE(std::numeric_limits<int>::min());
const int TOP_SCORE(std::numeric_limits<int>::max());

// Default thread parameter
const int MAX_THREADS = int(std::thread::hardware_concurrency());

class SearchEngine
{
public:
    // Heuristic function: gives score to each Board position
    Heuristic _heuristic;
    // MoveGenerator algorithm to create all possible tablut moves and hashes for the transposition Table
    MoveGenerator _moveGenerator;
    // Transposition table, used to store previous seen _board positions so we dont re-evaluate
    TranspositionTable _transpositionTable;
    // Timer for time limited search
    StopWatch _stopWatch;
    // To Check Board and hash current game boards
    Zobrist _zobrist;

    // _maxDepth set by the last method that invoked a XXSearch() Function
    int _maxDepth;

    // max score set by the last method that invoked a XXSearch() Function
    int _bestScore;

    // best move found by last method that invoked a XXSearch() Function
    Tablut _bestMove;

    SearchEngine(Heuristic __heuristic = Heuristic(), MoveGenerator __moveGenerator = MoveGenerator(), TranspositionTable __transpositionTable = TranspositionTable(), Zobrist _zobrist = Zobrist());
    ~SearchEngine();

    // NEGAMAX SEARCH ALGORITHM
    Tablut NegaMaxSearch(Tablut &__startingPosition, const int __maxDepth = 7, const int __threads = MAX_THREADS);
    Tablut NegaMaxSearchTimeLimited(Tablut &__startingPosition, const int __timeLimit, const int __threads = MAX_THREADS);

    Tablut NegaScoutSearch(Tablut &__startingPosition, const int __maxDepth = 7, const int __threads = MAX_THREADS);
    Tablut NegaScoutSearch2(Tablut &__startingPosition, const int __maxDepth = 7, const int __threads = MAX_THREADS);

    int NegaScoutParallel(Tablut &__currentMove, const int __depth, int __alpha, int __beta);
    int NegaScout(Tablut &__currentMove, const int __depth, int __alpha, int __beta);
    int NegaScoutTT(Tablut &__currentMove, const int __depth, int __alpha, int __beta);

    int NegaMax(Tablut &__currentMove, const int __depth, int __alpha, int __beta);
    int NegaMaxTimeLimited(Tablut &__currentMove, const int __depth, int __alpha, int __beta);

    int getTotalMoves();
    void resetTranspositionTable();
};

#endif