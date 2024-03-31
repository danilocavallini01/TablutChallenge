#ifndef SEARCHENGINE
#define SEARCHENGINE

#pragma once

#include "Tablut.h"
#include "Heuristic.h"
#include "Zobrist.h"
#include "TranspositionTable.h"

#include <limits>

const int BOTTOM_SCORE(std::numeric_limits<int>::min());
const int TOP_SCORE(std::numeric_limits<int>::max());

const int MAX_THREADS = 20;

class SearchEngine
{
public:
    // Heuristic function: gives score to each Board position
    Heuristic heuristic;
    // Zobrist algorithm to create HASH Key values for Transposition Table entries
    Zobrist zobrist;
    // Transposition table, used to store previous seen board positions so we dont re-evaluate
    TranspositionTable transpositionTable;

    // maxDepth set by the last method that invoked a XXSearch() Function
    int maxDepth;

    // max score set by the last method that invoked a XXSearch() Function
    int score;

    SearchEngine();
    SearchEngine(Heuristic &_heuristic, Zobrist &_zobrist, TranspositionTable &_transpositionTables);

    ~SearchEngine();

    Tablut NegaMaxSearch(Tablut &_startingPosition, const int _maxDepth, const int _color);
    Tablut NegaScoutSearch(Tablut &_startingPosition, const int _maxDepth);

    int NegaScout(Tablut &t, const int depth, int alpha, int beta);
    int NegaMax(Tablut &t, const int depth, int alpha, int beta, int color);
};

#endif