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
    Heuristic heuristic;
    Zobrist zobrist;
    TranspositionTable transpositionTable;
    
    int threads = 0;
    int totalThreads = 0;
    int maxDepth;

    Tablut bestmove;

    SearchEngine();
    SearchEngine(Heuristic &_heuristic, Zobrist &_zobrist, TranspositionTable &_transpositionTables);

    ~SearchEngine();

    Tablut search(Tablut &startingPosition, const int _maxDepth);
    int NegaScout(Tablut &t, const int depth, int alpha, int beta);
    int NegaMax(Tablut &t, const int depth, int alpha, int beta, int color);
};

#endif