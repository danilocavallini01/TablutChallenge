#include "SearchEngine.h"

SearchEngine::SearchEngine() {}

SearchEngine::SearchEngine(Heuristic &__heuristic, MoveGenerator &__moveGenerator, TranspositionTable &__transpositionTable)
{
    _heuristic = __heuristic;
    _moveGenerator = __moveGenerator;
    _transpositionTable = __transpositionTable;
}

SearchEngine::~SearchEngine(){};

Tablut SearchEngine::NegaMaxSearch(Tablut &__startingPosition, const int __maxDepth, const int __threads)
{
    _maxDepth = __maxDepth;

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

Tablut SearchEngine::NegaScoutSearch(Tablut &__startingPosition, const int __maxDepth, const int __threads)
{
    _maxDepth = __maxDepth;

    std::vector<Tablut> moves;
    Tablut bestMove;
    std::vector<std::future<int>> results;

    int alpha = BOTTOM_SCORE;
    int beta = TOP_SCORE;

    _bestScore = BOTTOM_SCORE;
    int b = beta;
    int v;

    _moveGenerator.generateLegalMoves(__startingPosition, moves);
    _heuristic.sortMoves(moves);

    bestMove = moves[0];

    for (int t = 0; t < moves.size(); t += __threads)
    {
        for (int i = 0; i < __threads && i + t < moves.size(); i++)
        {
            results.push_back(std::async(std::launch::async, &SearchEngine::NegaScout, std::ref(*this), std::ref(moves[i + t]), __maxDepth - 1, -b, -alpha));
        }

        for (int i = 0; i < results.size(); i++)
        {
            v = -results[i].get();

            if (v > alpha && v < beta && i > 0)
            {
                v = -SearchEngine::NegaScout(moves[i + t], __maxDepth - 1, -beta, -v);
            }

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
    ZobristKey hash = __currentMove.hash;
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

int SearchEngine::NegaMax(Tablut &__currentMove, const int __depth, int __alpha, int __beta)
{
    _totalMoves++;
    const int alphaOrigin = __alpha;
    int score;
    std::vector<Tablut> moves;
    Tablut move;

    // -------- TRANSPOSITION TABLE LOOKUP --------
    ZobristKey hash = __currentMove.hash;
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
