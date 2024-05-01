#include "SearchEngine.h"

std::atomic<int> _totalMoves;

// Total cutoof made by alpha beta prunings
std::atomic<int> _cutOffs[MAX_DEFAULT_DEPTH];

SearchEngine::SearchEngine(Heuristic __heuristic, Zobrist __zobrist, MoveGenerator __moveGenerator, TranspositionTable __transpositionTable)
{
    _heuristic = __heuristic;
    _moveGenerator = __moveGenerator;
    _transpositionTable = __transpositionTable;
    _zobrist = __zobrist;
}

SearchEngine::~SearchEngine(){};

// NEGASCOUT __________________________________________

Tablut SearchEngine::NegaScoutSearch(Tablut &__startingPosition, const int __maxDepth, const int __threads)
{
    _maxDepth = __maxDepth;
    _totalMoves = 0;
    _resetCutoffs();

    ZobristKey hash;

    std::vector<Tablut> moves;
    Tablut bestMove;
    std::vector<std::future<int>> results;

    _bestScore = BOTTOM_SCORE;
    int v;

    // GENERATE ALL LEGAL MOVES
    _moveGenerator.generateLegalMoves(__startingPosition, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        _zobrist.addHash(nextTablut, true);
    }

    // SORT MOVES
    _heuristic.sortMoves(moves, true);

    for (int t = 0; t < moves.size(); t += __threads)
    {
        for (int i = 0; i < __threads && i + t < moves.size(); i++)
        {
            results.push_back(std::async(std::launch::async, &SearchEngine::NegaScout, std::ref(*this), std::ref(moves[i + t]), __maxDepth - 1, BOTTOM_SCORE, TOP_SCORE));
        }

        for (int i = 0; i < results.size(); i++)
        {
            v = -results[i].get();

            // MAXIMIZE PROBLEM
            if (v > _bestScore)
            {
                bestMove = moves[i + t];
                _bestScore = v;
            }
        }

        results.clear();
    }

    return bestMove;
}

Tablut SearchEngine::NegaScoutSearchTimeLimited(Tablut &__startingPosition, StopWatch &_globalTimer, const int __threads)
{
    _maxDepth = MAX_DEFAULT_DEPTH;
    _totalMoves = 0;
    _resetCutoffs();

    ZobristKey hash;

    std::vector<Tablut> moves;
    Tablut bestMove;
    std::vector<std::future<int>> results;

    int totalMoves;

    _bestScore = BOTTOM_SCORE;
    int v;

    // GENERATE ALL LEGAL MOVES
    _moveGenerator.generateLegalMoves(__startingPosition, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        _zobrist.addHash(nextTablut);
        nextTablut.checkWinState();
    }

    // SORT MOVES
    _heuristic.sortMoves(moves, true, true);

    // TIME LIMIT SUBDIVISION
    totalMoves = moves.size();

    for (int t = 0; t < moves.size(); t += __threads)
    {
        // DIVIDE TIME BY GROUP OF THREADS, APPLY A TOLERANCE OF N% FOR EVERY GROUP OF THREAD;
        _computeSliceTimeLimit(_globalTimer, _stopWatch, totalMoves, __threads);
        _stopWatch.start();

        for (int i = 0; i < __threads && i + t < moves.size(); i++)
        {
            results.push_back(std::async(std::launch::async, &SearchEngine::NegaScoutTimeLimited, std::ref(*this), std::ref(moves[i + t]), _maxDepth - 1, BOTTOM_SCORE, TOP_SCORE));
        }

        for (int i = 0; i < results.size(); i++)
        {
            v = -results[i].get();

            if (v > _bestScore)
            {
                bestMove = moves[i + t];
                _bestScore = v;
            }
        }

        totalMoves -= __threads;

        results.clear();
        _stopWatch.reset();
    }

    return bestMove;
}

int SearchEngine::NegaScoutTT(Tablut &__currentMove, const int __depth, int __alpha, int __beta)
{
    _totalMoves++;

    const int alphaOrigin = __alpha;
    int score = BOTTOM_SCORE;
    int b;
    int v;

    std::vector<Tablut> moves;
    Tablut move;

    // -------- TRANSPOSITION TABLE LOOKUP --------
    ZobristKey hash = __currentMove._hash;
    std::optional<Entry> maybe_entry = _transpositionTable.get(hash);

    Entry tt_entry;

    if (maybe_entry.has_value())
    {
        tt_entry = maybe_entry.value();
        int tt_depth = std::get<ENTRY::DEPTH_INDEX>(tt_entry);

        if (tt_depth >= __depth)
        {
            _transpositionTable.cacheHit();

            FLAG tt_entry_flag = std::get<ENTRY::FLAG_INDEX>(tt_entry);
            int tt_score = std::get<ENTRY::SCORE_INDEX>(tt_entry);

            if (tt_entry_flag == FLAG::EXACT)
            {
                return tt_score;
            }
            else if (tt_entry_flag == FLAG::LOWERBOUND)
            {
                __alpha = std::max(__alpha, tt_score);
            }
            else if (tt_entry_flag == FLAG::UPPERBOUND)
            {
                __beta = std::min(__beta, tt_score);
            }

            if (__alpha >= __beta)
            {
                _cutOffs[__depth]++;
                return tt_score;
            }
        }
    }

    // --------TRANSPOSITION TABLE LOOKUP -------- END

    if (__currentMove.isGameOver() || __depth == 0)
    {
        return _heuristic.evaluate(__currentMove, true);
    }

    score = BOTTOM_SCORE;
    b = __beta;

    _moveGenerator.generateLegalMoves(__currentMove, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        _zobrist.addHash(nextTablut, true);
    }

    // LOSE BY NO MOVE LEFT
    if (moves.size() == 0)
    {
        if (__currentMove._isWhiteTurn)
        {
            __currentMove._gameState = GAME_STATE::BLACKWIN;
        }
        else
        {
            __currentMove._gameState = GAME_STATE::WHITEWIN;
        }

        return _heuristic.evaluate(__currentMove, true);
    }

    // SORT MOVES
    _heuristic.sortMoves(moves, true);

    // NEGASCOUT CORE ENGINE
    for (int i = 0; i < moves.size(); i++)
    {
        move = moves[i];
        v = -SearchEngine::NegaScoutTT(move, __depth - 1, -b, -__alpha);

        if (v > __alpha && v < __beta && i > 0)
        {
            v = -SearchEngine::NegaScoutTT(move, __depth - 1, -__beta, -v);
        }

        if (v > score)
        {
            score = v;
        }

        __alpha = std::max(__alpha, v);

        if (__alpha >= __beta)
        {
            _cutOffs[__depth]++;
            break;
        }

        b = __alpha + 1;
    }

    // -------- TRANSPOSITION TABLE PUT --------

    if (score <= alphaOrigin)
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::LOWERBOUND);
    }
    else if (score >= b)
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::UPPERBOUND);
    }
    else
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::EXACT);
    }

    _transpositionTable.put(tt_entry, hash);
    _transpositionTable.cachePut();

    // -------- TRANSPOSITION TABLE PUT -------- END

    return score;
}

int SearchEngine::NegaScout(Tablut &__currentMove, const int __depth, int __alpha, int __beta)
{
    _totalMoves++;

    const int alphaOrigin = __alpha;
    int score = BOTTOM_SCORE;
    int b;
    int v;

    std::vector<Tablut> moves;
    Tablut move;

    if (__currentMove.isGameOver() || __depth == 0)
    {
        return _heuristic.evaluate(__currentMove, true);
    }

    score = BOTTOM_SCORE;
    b = __beta;

    _moveGenerator.generateLegalMoves(__currentMove, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        _zobrist.addHash(nextTablut, true);
    }

    // LOSE BY NO MOVE LEFT
    if (moves.size() == 0)
    {
        if (__currentMove._isWhiteTurn)
        {
            __currentMove._gameState = GAME_STATE::BLACKWIN;
        }
        else
        {
            __currentMove._gameState = GAME_STATE::WHITEWIN;
        }

        return _heuristic.evaluate(__currentMove, true);
    }

    // SORT MOVES
    _heuristic.sortMoves(moves, true);

    // NEGASCOUT CORE ENGINE
    for (int i = 0; i < moves.size(); i++)
    {
        move = moves[i];
        v = -SearchEngine::NegaScout(move, __depth - 1, -b, -__alpha);

        if (v > __alpha && v < __beta && i > 0)
        {
            v = -SearchEngine::NegaScout(move, __depth - 1, -__beta, -v);
        }

        if (v > score)
        {
            score = v;
        }

        __alpha = std::max(__alpha, v);

        if (__alpha >= __beta)
        {
            _cutOffs[__depth]++;
            break;
        }

        b = __alpha + 1;
    }

    return score;
}

int SearchEngine::NegaScoutTimeLimited(Tablut &__currentMove, const int __depth, int __alpha, int __beta)
{
    _totalMoves++;

    const int alphaOrigin = __alpha;
    int score = BOTTOM_SCORE;
    int b;
    int v;

    std::vector<Tablut> moves;
    Tablut move;

    if (_stopWatch.isTimeouted() || __currentMove.isGameOver() || __depth == 0)
    {
        return _heuristic.evaluate(__currentMove, true);
    }

    score = BOTTOM_SCORE;
    b = __beta;

    _moveGenerator.generateLegalMoves(__currentMove, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        _zobrist.addHash(nextTablut, true);
        nextTablut.checkWinState();
    }

    // LOSE BY NO MOVE LEFT
    if (moves.size() == 0)
    {
        if (__currentMove._isWhiteTurn)
        {
            __currentMove._gameState = GAME_STATE::BLACKWIN;
        }
        else
        {
            __currentMove._gameState = GAME_STATE::WHITEWIN;
        }

        return _heuristic.evaluate(__currentMove, true);
    }

    // SORT MOVES
    _heuristic.sortMoves(moves, true);

    // NEGASCOUT CORE ENGINE
    for (int i = 0; i < moves.size(); i++)
    {
        move = moves[i];
        v = -SearchEngine::NegaScoutTimeLimited(move, __depth - 1, -b, -__alpha);

        if (v > __alpha && v < __beta && i > 0)
        {
            v = -SearchEngine::NegaScoutTimeLimited(move, __depth - 1, -__beta, -v);
        }

        if (v > score)
        {
            if (__depth == _maxDepth)
            {
                _bestScore = v;
            }
            score = v;
        }

        __alpha = std::max(__alpha, v);

        if (__alpha >= __beta)
        {
            _cutOffs[__depth]++;
            break;
        }

        b = __alpha + 1;
    }

    return score;
}

Tablut SearchEngine::AlphaBetaSearch(Tablut &__startingPosition, const int __maxDepth, const int __threads)
{
    _maxDepth = __maxDepth;
    _totalMoves = 0;
    _resetCutoffs();

    ZobristKey hash;

    std::vector<Tablut> moves;
    Tablut bestMove;
    std::vector<std::future<int>> results;

    int alpha = BOTTOM_SCORE;
    int beta = TOP_SCORE;

    _bestScore = BOTTOM_SCORE;
    int v;

    // GENERATE ALL LEGAL MOVES
    _moveGenerator.generateLegalMoves(__startingPosition, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        _zobrist.addHash(nextTablut);

        nextTablut.checkWinState();
    }

    bestMove = moves[0];

    _heuristic.sortMoves(moves, !__startingPosition._isWhiteTurn);

    if (__startingPosition._isWhiteTurn)
    {
        v = BOTTOM_SCORE;
        _bestScore = v;

        for (int t = 0; t < moves.size(); t += __threads)
        {
            for (int i = 0; i < __threads && i + t < moves.size(); i++)
            {
                results.push_back(std::async(std::launch::async, &SearchEngine::AlphaBeta, std::ref(*this), std::ref(moves[i + t]), __maxDepth - 1, alpha, beta, false));
            }

            for (int i = 0; i < results.size(); i++)
            {
                v = std::max(v, results[i].get());
                if (v > _bestScore)
                {
                    bestMove = moves[i + t];
                    _bestScore = v;
                }

                if (v > alpha)
                {
                    alpha = v;
                }
            }

            results.clear();
        }
    }
    else
    {
        v = TOP_SCORE;
        _bestScore = v;

        for (int t = 0; t < moves.size(); t += __threads)
        {
            for (int i = 0; i < __threads && i + t < moves.size(); i++)
            {
                results.push_back(std::async(std::launch::async, &SearchEngine::AlphaBeta, std::ref(*this), std::ref(moves[i + t]), __maxDepth - 1, alpha, beta, true));
            }

            for (int i = 0; i < results.size(); i++)
            {
                v = std::min(v, results[i].get());
                if (v < _bestScore)
                {
                    bestMove = moves[i + t];
                    _bestScore = v;
                }

                if (v < beta)
                {
                    beta = v;
                    bestMove = moves[i + t];
                    _bestScore = v;
                }
            }

            results.clear();
        }
    }

    return bestMove;
}

int SearchEngine::AlphaBeta(Tablut &__currentMove, const int __depth, int __alpha, int __beta, bool isWhite)
{
    const int alphaOrigin = __alpha;
    _totalMoves++;

    int score;
    std::vector<Tablut> moves;
    Tablut move;

    // -------- TRANSPOSITION TABLE LOOKUP --------
    ZobristKey hash = __currentMove._hash;
    std::optional<Entry> maybe_entry = _transpositionTable.get(hash);

    Entry tt_entry;

    if (maybe_entry.has_value())
    {
        _transpositionTable.cacheHit();
        tt_entry = maybe_entry.value();
        int tt_depth = std::get<ENTRY::DEPTH_INDEX>(tt_entry);

        if (tt_depth >= __depth)
        {
            FLAG tt_entry_flag = std::get<ENTRY::FLAG_INDEX>(tt_entry);
            int tt_score = std::get<ENTRY::SCORE_INDEX>(tt_entry);

            if (tt_entry_flag == FLAG::EXACT)
            {
                return tt_score;
            }
            else if (tt_entry_flag == FLAG::LOWERBOUND)
            {
                __alpha = std::max(__alpha, tt_score);
            }
            else
            {
                __beta = std::min(__beta, tt_score);
            }

            if (__alpha >= __beta)
            {
                _cutOffs[__depth]++;
                return tt_score;
            }
        }
    }
    // --------TRANSPOSITION TABLE LOOKUP -------- END

    if (__currentMove.isGameOver() || __depth == 0)
    {
        return _heuristic.evaluate(__currentMove);
    }

    _moveGenerator.generateLegalMoves(__currentMove, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        nextTablut._hash = _zobrist.hash(nextTablut, true);
        _zobrist.addHash(nextTablut);

        nextTablut.checkWinState();
    }

    // LOSE BY NO MOVE LEFT
    if (moves.size() == 0)
    {
        if (__currentMove._isWhiteTurn)
        {
            __currentMove._gameState = GAME_STATE::BLACKWIN;
        }
        else
        {
            __currentMove._gameState = GAME_STATE::WHITEWIN;
        }

        return _heuristic.evaluate(__currentMove);
    }

    _heuristic.sortMoves(moves, !isWhite);

    if (isWhite)
    {
        score = BOTTOM_SCORE;

        for (int i = 0; i < moves.size(); i++)
        {
            score = std::max(score, AlphaBeta(moves[i], __depth - 1, __alpha, __beta, false));

            if (score > __alpha)
            {
                __alpha = score;
            }

            if (score >= __beta)
            {
                _cutOffs[__depth]++;
                break;
            }
        }
    }
    else
    {
        score = TOP_SCORE;

        for (int i = 0; i < moves.size(); i++)
        {
            score = std::min(score, AlphaBeta(moves[i], __depth - 1, __alpha, __beta, true));

            if (score < __beta)
            {
                __beta = score;
            }

            if (score <= __alpha)
            {
                _cutOffs[__depth]++;
                break;
            }
        }
    }

    // -------- TRANSPOSITION TABLE PUT --------

    if (score <= alphaOrigin)
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::UPPERBOUND);
    }
    else if (score >= __beta)
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::LOWERBOUND);
    }
    else
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::EXACT);
    }

    _transpositionTable.put(tt_entry, hash);
    _transpositionTable.cachePut();

    // -------- TRANSPOSITION TABLE PUT -------- END

    return score;
}

int SearchEngine::getTotalMoves()
{
    return _totalMoves;
}

void SearchEngine::resetTranspositionTable()
{
    _transpositionTable.clear();
}

void SearchEngine::_resetCutoffs()
{
    for (int i = 0; i < MAX_DEFAULT_DEPTH; i++)
    {
        _cutOffs[i] = 0;
    }
}

int SearchEngine::getCutOffs(int index)
{
    return _cutOffs[index];
}

void SearchEngine::_computeSliceTimeLimit(StopWatch &__globalTimer, StopWatch &__mustSetTimer, int __remainingMoves, int __threads)
{
    int slicedTimeLimit = int(float(__globalTimer.getRemainingTime()) / std::ceil(float(__remainingMoves) / float(__threads)) * (100.0 - MAX_TIME_ERROR) / 100.0);
    std::cout << "TIME LIMIT SLICED -> TR: " << __globalTimer.getRemainingTime() << ", REMAINING MOVES:" << __remainingMoves << ", TOTAL THREAD: " << __threads << ", SLICED TIME: " << slicedTimeLimit << std::endl;
    __mustSetTimer.setTimeLimit(slicedTimeLimit);
}

/*

Tablut SearchEngine::NegaScoutSearchSliced(Tablut &__startingPosition, const int __maxDepth, const int __threads)
{
    _maxDepth = __maxDepth;
    _totalMoves = 0;
    _resetCutoffs();

    ZobristKey hash;

    std::vector<Tablut> moves;
    Tablut bestMove;
    std::vector<std::future<std::pair<int,Tablut>>> results;

    _bestScore = BOTTOM_SCORE;
    std::pair<int,Tablut> v;

    // GENERATE ALL LEGAL MOVES
    _moveGenerator.generateLegalMoves(__startingPosition, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        _zobrist.addHash(nextTablut, true);
    }

    // SORT MOVES
    _heuristic.sortMoves(moves, true);

    int movesPerThread = moves.size() / __threads;

    for (int t = 0; t < moves.size(); t += movesPerThread)
    {
        // LAST ITERATION, CONSUME ALL MOVES VECTOR
        if (moves.size() <= t + movesPerThread * 2)
        {
            results.push_back(std::async(std::launch::async, &SearchEngine::NegaScoutSliced, std::ref(*this), std::ref(moves), t, moves.size() - 1, _maxDepth - 1));
            break;
        }
        else
        {
            results.push_back(std::async(std::launch::async, &SearchEngine::NegaScoutSliced, std::ref(*this), std::ref(moves), t, t + movesPerThread - 1, _maxDepth - 1));
        }
    }

    for (int i = 0; i < results.size(); i++)
    {
        v = results[i].get();

        // MAXIMIZE PROBLEM
        if (v.first > _bestScore)
        {
            bestMove = v.second;
            _bestScore = v.first;
        }
    }

    return bestMove;
}

std::pair<int,Tablut> SearchEngine::NegaScoutSliced(std::vector<Tablut> &__moves, int __startIndex, int __endIndex, const int __depth)
{
    int alpha = BOTTOM_SCORE;
    int beta = TOP_SCORE;

    int score = BOTTOM_SCORE;
    int b = beta;
    int v;

    Tablut move;
    Tablut bestMove;

    // NEGASCOUT CORE ENGINE
    for (int i = __startIndex; i <= __endIndex; i++)
    {
        move = __moves[i];
        v = -SearchEngine::NegaScout(move, __depth, -b, -alpha);

        if (v > alpha && v < beta && i > __startIndex)
        {
            v = -SearchEngine::NegaScout(move, __depth, -beta, -v);
        }
        std::cout << "SCORE i:"<< i << "->"<< v << std::endl;

        if (v > score)
        {
            bestMove = move;
            score = v;
        }

        alpha = std::max(alpha, v);

        if (alpha >= beta)
        {
            _cutOffs[__depth]++;
            break;
        }

        b = alpha + 1;
    }

    return { score, bestMove };
}*/
