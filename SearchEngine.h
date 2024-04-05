#pragma once

#ifndef SEARCHENGINE
#define SEARCHENGINE

#include "Tablut.h"
#include "Heuristic.h"
#include "MoveGenerator.h"
#include "TranspositionTable.h"

#include <limits>
#include <thread>
#include <cstring>
#include <future>

// Forward Declaration
class Tablut;
class Heuristic;
class MoveGenerator;
class TranspositionTable;

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

    // _maxDepth set by the last method that invoked a XXSearch() Function
    int _maxDepth;

    // max score set by the last method that invoked a XXSearch() Function
    int _bestScore;

    // totalMoves checked by search engine
    int _totalMoves;

    SearchEngine();
    SearchEngine(Heuristic &__heuristic, MoveGenerator &__moveGenerator, TranspositionTable &__transpositionTable);

    ~SearchEngine();

    Tablut NegaMaxSearch(Tablut &__startingPosition, const int __maxDepth = 7, const int __threads = MAX_THREADS);
    Tablut NegaScoutSearch(Tablut &__startingPosition, const int __maxDepth = 7, const int __threads = MAX_THREADS);

    int NegaScout(Tablut &__currentMove, const int __depth, int __alpha, int __beta);
    int NegaMax(Tablut &__currentMove, const int __depth, int __alpha, int __beta);
};

#endif