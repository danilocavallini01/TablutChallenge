#include "SearchEngine.h"

std::atomic<int> _totalMoves;

SearchEngine::SearchEngine(Heuristic __heuristic, MoveGenerator __moveGenerator, TranspositionTable __transpositionTable, Zobrist __zobrist)
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
    ZobristKey hash;

    std::vector<Tablut> moves;
    Tablut bestMove;
    std::vector<std::future<int>> results;

    std::chrono::steady_clock::time_point begin, end;

    int alpha = BOTTOM_SCORE;
    int beta = TOP_SCORE;

    _bestScore = BOTTOM_SCORE;
    int b = beta;
    int v = BOTTOM_SCORE;

    // SET FIRST SCORE OF FIRST TABLUT
    __startingPosition._score = _heuristic.evaluate(__startingPosition);

    // GENERATE ALL LEGAL MOVES
    _moveGenerator.generateLegalMoves(__startingPosition, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        hash = _zobrist.hash(nextTablut);

        nextTablut._hash = hash;
        nextTablut._pastHashes[nextTablut._pastHashesIndex++] = hash;

        if (nextTablut._pastHashesIndex == MAX_DRAW_LOG)
        {
            nextTablut._pastHashesIndex = 0;
        }

        nextTablut.checkWinState();
    }

    // SORT MOVES

    begin = std::chrono::steady_clock::now();
    _heuristic.sortMoves(moves);
    end = std::chrono::steady_clock::now();

    std::cout << "HEURISTIC TIME: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;

    bestMove = moves[0];

    for (int t = 0; t < moves.size(); t += __threads)
    {
        for (int i = 0; i < __threads && i + t < moves.size(); i++)
        {
            if (v > alpha && v < beta && i + t > 0)
            {
                results.push_back(std::async(std::launch::async, &SearchEngine::NegaScout, std::ref(*this), std::ref(moves[i + t]), __maxDepth - 1, -beta, -v));
            }
            else
            {
                results.push_back(std::async(std::launch::async, &SearchEngine::NegaScout, std::ref(*this), std::ref(moves[i + t]), __maxDepth - 1, -b, -alpha));
            }
        }

        for (int i = 0; i < results.size(); i++)
        {
            v = -results[i].get();

            if (v > _bestScore)
            {

                bestMove = moves[i + t];
                _bestScore = v;
            }

            alpha = std::max(alpha, v);

            if (alpha >= beta)
            {
                break;
            }

            b = alpha + 1;
        }

        results.clear();
    }

    return bestMove;
}

Tablut SearchEngine::NegaScoutSearchTimeLimited(Tablut &__startingPosition, StopWatch &_globalTimer, const int __threads)
{
    _maxDepth = MAX_DEFAULT_DEPTH;
    _totalMoves = 0;
    ZobristKey hash;

    std::vector<Tablut> moves;
    Tablut bestMove;
    std::vector<std::future<int>> results;
    int totalMoves;

    std::chrono::steady_clock::time_point begin, end;

    int alpha = BOTTOM_SCORE;
    int beta = TOP_SCORE;

    _bestScore = BOTTOM_SCORE;
    int b = beta;
    int v = BOTTOM_SCORE;

    // GENERATE AND SORT MOVES
    _moveGenerator.generateLegalMoves(__startingPosition, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        hash = _zobrist.hash(nextTablut);

        nextTablut._hash = hash;
        nextTablut._pastHashes[nextTablut._pastHashesIndex++] = hash;

        if (nextTablut._pastHashesIndex == MAX_DRAW_LOG)
        {
            nextTablut._pastHashesIndex = 0;
        }

        nextTablut.checkWinState();
    }

    begin = std::chrono::steady_clock::now();
    _heuristic.sortMoves(moves);
    end = std::chrono::steady_clock::now();

    std::cout << "HEURISTIC TIME: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;

    // TIME LIMIT SUBDIVISION
    totalMoves = moves.size();

    bestMove = moves[0];

    for (int t = 0; t < moves.size(); t += __threads)
    {
        // DIVIDE TIME BY GROUP OF THREADS, APPLY A TOLERANCE OF N% FOR EVERY GROUP OF THREAD;
        _computeSliceTimeLimit(_globalTimer, _stopWatch, totalMoves, __threads);
        _stopWatch.start();

        for (int i = 0; i < __threads && i + t < moves.size(); i++)
        {
            if (v > alpha && v < beta && i + t > 0)
            {
                results.push_back(std::async(std::launch::async, &SearchEngine::NegaScoutTimeLimited, std::ref(*this), std::ref(moves[i + t]), _maxDepth - 1, -beta, -v));
            }
            else
            {
                results.push_back(std::async(std::launch::async, &SearchEngine::NegaScoutTimeLimited, std::ref(*this), std::ref(moves[i + t]), _maxDepth - 1, -b, -alpha));
            }
        }

        for (int i = 0; i < results.size(); i++)
        {
            v = -results[i].get();
            if (v > _bestScore)
            {

                bestMove = moves[i + t];
                _bestScore = v;
            }

            alpha = std::max(alpha, v);

            if (alpha >= beta)
            {
                return bestMove;
            }

            b = alpha + 1;
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
    int score;
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
                return tt_score;
            }
        }
    }
    // --------TRANSPOSITION TABLE LOOKUP -------- END

    if (__currentMove.isGameOver() || __depth == 0)
    {
        return _heuristic.evaluate(__currentMove);
    }

    score = BOTTOM_SCORE;
    b = __beta;

    _moveGenerator.generateLegalMoves(__currentMove, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        hash = _zobrist.hash(nextTablut);

        nextTablut._hash = hash;
        nextTablut._pastHashes[nextTablut._pastHashesIndex++] = hash;

        if (nextTablut._pastHashesIndex == MAX_DRAW_LOG)
        {
            nextTablut._pastHashesIndex = 0;
        }

        nextTablut.checkWinState();
    }

    // LOSE BY NO MOVE LEFT
    if (moves.size() == 0)
    {
        if (__currentMove._isWhiteTurn)
        {
            __currentMove._gameState == GAME_STATE::BLACKWIN;
        }
        else
        {
            __currentMove._gameState == GAME_STATE::WHITEWIN;
        }

        return _heuristic.evaluate(__currentMove);
    }

    // SORT MOVES
    _heuristic.sortMoves(moves);

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
            break;
        }

        b = __alpha + 1;
    }

    // -------- TRANSPOSITION TABLE PUT --------

    if (score <= alphaOrigin)
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::UPPERBOUND);
    }
    else if (score >= b)
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

int SearchEngine::NegaScout(Tablut &__currentMove, const int __depth, int __alpha, int __beta)
{
    _totalMoves++;

    const int alphaOrigin = __alpha;
    int score;
    int b;
    int v;

    std::vector<Tablut> moves;
    Tablut move;

    // -------- TRANSPOSITION TABLE LOOKUP --------
    ZobristKey hash = __currentMove._hash;

    /*
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
                return tt_score;
            }
        }
    }
    // --------TRANSPOSITION TABLE LOOKUP -------- END
*/

    if (__currentMove.isGameOver() || __depth == 0)
    {
        return _heuristic.evaluate(__currentMove);
    }

    score = BOTTOM_SCORE;
    b = __beta;

    _moveGenerator.generateLegalMoves(__currentMove, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        hash = _zobrist.hash(nextTablut);

        nextTablut._hash = hash;
        nextTablut._pastHashes[nextTablut._pastHashesIndex++] = hash;

        if (nextTablut._pastHashesIndex == MAX_DRAW_LOG)
        {
            nextTablut._pastHashesIndex = 0;
        }

        nextTablut.checkWinState();
    }

    // LOSE BY NO MOVE LEFT
    if (moves.size() == 0)
    {
        if (__currentMove._isWhiteTurn)
        {
            __currentMove._gameState == GAME_STATE::BLACKWIN;
        }
        else
        {
            __currentMove._gameState == GAME_STATE::WHITEWIN;
        }

        return _heuristic.evaluate(__currentMove);
    }

    // SORT MOVES
    _heuristic.sortMoves(moves);

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
            break;
        }

        b = __alpha + 1;
    }

    /*

    // -------- TRANSPOSITION TABLE PUT --------
    if (score <= alphaOrigin)
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::UPPERBOUND);
    }
    else if (score >= b)
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::LOWERBOUND);
    }
    else
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::EXACT);
    }

    _transpositionTable.put(tt_entry, hash);
    _transpositionTable.cachePut();

    */

    // -------- TRANSPOSITION TABLE PUT -------- END

    return score;
}

int SearchEngine::NegaScoutTimeLimited(Tablut &__currentMove, const int __depth, int __alpha, int __beta)
{
    _totalMoves++;

    const int alphaOrigin = __alpha;
    int score;
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
                return tt_score;
            }
        }
    }
    // --------TRANSPOSITION TABLE LOOKUP -------- END

    if (_stopWatch.isTimeouted() || __currentMove.isGameOver() || __depth == 0)
    {
        return _heuristic.evaluate(__currentMove);
    }

    score = BOTTOM_SCORE;
    b = __beta;

    _moveGenerator.generateLegalMoves(__currentMove, moves);

    // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
    for (auto &nextTablut : moves)
    {
        hash = _zobrist.hash(nextTablut);

        nextTablut._hash = hash;
        nextTablut._pastHashes[nextTablut._pastHashesIndex++] = hash;

        if (nextTablut._pastHashesIndex == MAX_DRAW_LOG)
        {
            nextTablut._pastHashesIndex = 0;
        }

        nextTablut.checkWinState();
    }

    // LOSE BY NO MOVE LEFT
    if (moves.size() == 0)
    {
        if (__currentMove._isWhiteTurn)
        {
            __currentMove._gameState == GAME_STATE::BLACKWIN;
        }
        else
        {
            __currentMove._gameState == GAME_STATE::WHITEWIN;
        }

        return _heuristic.evaluate(__currentMove);
    }

    // SORT MOVES
    _heuristic.sortMoves(moves);

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
            score = v;
        }

        __alpha = std::max(__alpha, v);

        if (__alpha >= __beta)
        {
            break;
        }

        b = __alpha + 1;
    }

    // -------- TRANSPOSITION TABLE PUT --------
    if (score <= alphaOrigin)
    {
        tt_entry = std::make_tuple(score, __depth, FLAG::UPPERBOUND);
    }
    else if (score >= b)
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

// NEGAMAX __________________________________________

Tablut SearchEngine::NegaMaxSearch(Tablut &__startingPosition, const int __maxDepth, const int __threads)
{
    _maxDepth = __maxDepth;
    _totalMoves = 0;

    std::vector<Tablut> moves;
    Tablut bestMove;
    std::vector<std::future<int>> results;

    int alpha = BOTTOM_SCORE;
    int beta = TOP_SCORE;

    _bestScore = BOTTOM_SCORE;
    int v;

    _moveGenerator.generateLegalMoves(__startingPosition, moves);
    _heuristic.sortMoves(moves);

    bestMove = moves[0];

    for (int t = 0; t < moves.size(); t += __threads)
    {
        for (int i = 0; i < __threads && i + t < moves.size(); i++)
        {
            results.push_back(std::async(std::launch::async, &SearchEngine::NegaMax, std::ref(*this), std::ref(moves[i + t]), __maxDepth - 1, -beta, -alpha));
        }

        for (int i = 0; i < results.size(); i++)
        {
            v = -results[i].get();

            if (v > _bestScore)
            {
                bestMove = moves[i + t];
                _bestScore = v;
            }

            alpha = std::max(alpha, _bestScore);
            if (alpha >= beta)
            {
                break;
            }
        }

        results.clear();
    }

    return bestMove;
}

Tablut SearchEngine::NegaMaxSearchTimeLimited(Tablut &__startingPosition, const int __timeLimit, const int __threads)
{
    _totalMoves = 0;

    std::vector<Tablut> moves;
    Tablut bestMove;
    std::vector<std::future<int>> results;

    int alpha = BOTTOM_SCORE;
    int beta = TOP_SCORE;

    _bestScore = BOTTOM_SCORE;
    int v;

    _moveGenerator.generateLegalMoves(__startingPosition, moves);
    _heuristic.sortMoves(moves);

    // TIME LIMIT SUBDIVISION

    // DIVIDE TIME BY GROUP OF THREADS, APPLY A TOLERANCE OF 17% FOR EVERY GROUP OF THREAD;
    int slicedTimeLimit = __timeLimit / (moves.size() / __threads) * 0.80;

    std::cout << "TIME LIMIT SLICED ->" << moves.size() << "-" << __threads << "-" << slicedTimeLimit << std::endl;

    _stopWatch.setTimeLimit(slicedTimeLimit);

    bestMove = moves[0];

    for (int t = 0; t < moves.size(); t += __threads)
    {
        _stopWatch.start();

        for (int i = 0; i < __threads && i + t < moves.size(); i++)
        {
            results.push_back(std::async(std::launch::async, &SearchEngine::NegaMaxTimeLimited, std::ref(*this), std::ref(moves[i + t]), MAX_DEFAULT_DEPTH, -beta, -alpha));
        }

        for (int i = 0; i < results.size(); i++)
        {
            v = -results[i].get();

            if (v > _bestScore)
            {
                bestMove = moves[i + t];
                _bestScore = v;
            }

            alpha = std::max(alpha, _bestScore);
            if (alpha >= beta)
            {
                break;
            }
        }

        results.clear();
        _stopWatch.reset();
    }

    return bestMove;
}

int SearchEngine::NegaMax(Tablut &__currentMove, const int __depth, int __alpha, int __beta)
{
    _totalMoves++;
    const int alphaOrigin = __alpha;
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

    // LOSE BY NO MOVE LEFT
    if (moves.size() == 0)
    {
        if (__currentMove._isWhiteTurn)
        {
            __currentMove._gameState == GAME_STATE::BLACKWIN;
        }
        else
        {
            __currentMove._gameState == GAME_STATE::WHITEWIN;
        }

        return __currentMove._isWhiteTurn ? _heuristic.evaluate(__currentMove) : -_heuristic.evaluate(__currentMove);
    }

    _heuristic.sortMoves(moves);
    score = BOTTOM_SCORE;

    for (int i = 0; i < moves.size(); i++)
    {
        score = std::max(score, -NegaMax(moves[i], __depth - 1, -__beta, -__alpha));
        __alpha = std::max(__alpha, score);
        if (__alpha >= __beta)
        {
            break;
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

    return score;
}

int SearchEngine::NegaMaxTimeLimited(Tablut &__currentMove, const int __depth, int __alpha, int __beta)
{
    const int alphaOrigin = __alpha;
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
                return tt_score;
            }
        }
    }
    // --------TRANSPOSITION TABLE LOOKUP -------- END

    if (_stopWatch.isTimeouted() || __currentMove.isGameOver() || __depth == 0)
    {
        return _heuristic.evaluate(__currentMove);
    }

    _moveGenerator.generateLegalMoves(__currentMove, moves);

    // LOSE BY NO MOVE LEFT
    if (moves.size() == 0)
    {
        if (__currentMove._isWhiteTurn)
        {
            __currentMove._gameState == GAME_STATE::BLACKWIN;
        }
        else
        {
            __currentMove._gameState == GAME_STATE::WHITEWIN;
        }

        return _heuristic.evaluate(__currentMove);
    }

    _heuristic.sortMoves(moves);
    score = BOTTOM_SCORE;

    for (int i = 0; i < moves.size(); i++)
    {
        score = std::max(score, -NegaMaxTimeLimited(moves[i], __depth - 1, -__beta, -__alpha));
        __alpha = std::max(__alpha, score);
        if (__alpha >= __beta)
        {
            break;
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

void SearchEngine::_computeSliceTimeLimit(StopWatch &__globalTimer, StopWatch &__mustSetTimer, int __remainingMoves, int __threads)
{
    int slicedTimeLimit = int(float(__globalTimer.getRemainingTime()) / std::ceil(float(__remainingMoves) / float(__threads)) * ( 100.0 - MAX_TIME_ERROR) / 100.0) ;
    std::cout << "TIME LIMIT SLICED -> TR: " << __globalTimer.getRemainingTime() << ", REMAINING MOVES:" << __remainingMoves << ", TOTAL THREAD: " << __threads << ", SLICED TIME: " << slicedTimeLimit << std::endl;
    __mustSetTimer.setTimeLimit(slicedTimeLimit);
}