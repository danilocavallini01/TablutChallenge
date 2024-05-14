#ifndef NEGASCOUT_SEARCH_ENGINE
#define NEGASCOUT_SEARCH_ENGINE

#include "AbstractSearchEngine.hpp"

class NegaScoutEngine : public AbstractSearchEngine
{
public:
    NegaScoutEngine(Heuristic __heuristic, Zobrist __zobrist, int __maxDepth = MAX_DEFAULT_DEPTH, int __quiescenceMaxDepth = MAX_DEFAULT_QSEARCH_DEPTH)
        : AbstractSearchEngine(__maxDepth, __quiescenceMaxDepth, true, __heuristic, __zobrist)
    {
    }

    ~NegaScoutEngine(){};

    // NEGASCOUT __________________________________________
    Tablut Search(Tablut &__startingPosition)
    {
        reset();
        resetTranspositionTable();

        NegaScout(__startingPosition, _maxDepth, BOTTOM_SCORE, TOP_SCORE, __startingPosition._isWhiteTurn);

        return _bestMove;
    }

    Tablut ParallelSearch(Tablut &__startingPosition, int __threads = MAX_THREADS)
    {
        reset();
        resetTranspositionTable();

        std::vector<Tablut> moves;
        std::vector<std::future<int>> results;

        const bool color = __startingPosition._isWhiteTurn;

        _bestScore = BOTTOM_SCORE;
        int alpha = BOTTOM_SCORE;
        int beta = TOP_SCORE;
        int b = beta;

        int v;

        // GENERATE ALL LEGAL MOVES
        getMoves(__startingPosition, moves);

        // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
        addHashToMoves(moves);

        // SORT MOVES
        sortMoves(moves, _maxDepth, color);

        for (int t = 0; t < moves.size(); t += __threads)
        {
            for (int i = 0; i < __threads && i + t < moves.size(); i++)
            {
                results.push_back(std::async(std::launch::async, &NegaScoutEngine::NegaScout, this, std::ref(moves[i + t]), _maxDepth - 1, -b, -alpha, !color));
            }

            for (int i = 0; i < results.size(); i++)
            {
                v = -results[i].get();

                if (v > alpha && v < beta && i + t > 0)
                {
                    v = -NegaScout(moves[i + t], _maxDepth - 1, -beta, -alpha, !color);
                }

                if (v > _bestScore || t + i == 0)
                {
                    _bestMove = moves[i + t];
                    _bestScore = v;
                }

                alpha = std::max(alpha, v);

                b = alpha + 1;
            }

            results.clear();
        }

        return _bestMove;
    }

    Tablut TimeLimitedSearch(Tablut &__startingPosition, StopWatch &_globalTimer, int __threads = MAX_THREADS)
    {
        reset();

        _stopWatch = _globalTimer;

        NegaScoutTimeLimited(__startingPosition, _maxDepth, BOTTOM_SCORE, TOP_SCORE, __startingPosition._isWhiteTurn);

        return _bestMove;
    }

    int Quiesce(Tablut &__currentMove, int __qDepth, int __alpha, int __beta, int __color)
    {

        // QUIESCENCE MAX DEPTH OR GAME OVER CONDITION
        if (__currentMove.isGameOver() || __qDepth == _quiesceMaxDepth)
        {
            return evaluate(__currentMove, __qDepth, __color);
        }

        _qTotalMoves++;

        int score = BOTTOM_SCORE;
        std::vector<Tablut> moves;

        int standPat = evaluate(__currentMove, __qDepth, __color);

        if (standPat >= __beta)
        {
            return __beta;
        }

        __alpha = std::max(__alpha, standPat);

        // GENERATE ALL NON QUIET MOVES ( CAPTURE OR WINS )
        getMoves(__currentMove, moves);

        // NO MOVE AVAILABLE, LOSE BY NO MOVE LEFT
        if (moves.size() == 0)
        {
            if (__color)
            {
                __currentMove._gameState = GAME_STATE::BLACKWIN;
            }
            else
            {
                __currentMove._gameState = GAME_STATE::WHITEWIN;
            }

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

        // NO NON-QUIET MOVE AVAILABLE, QUIET STATE SO WE RETURN NORMAL HEURISTIC VALUE
        if (score == BOTTOM_SCORE)
        {
            return evaluate(__currentMove, __qDepth, __color);
        }

        return __alpha;
    }

    int QuiesceTimeLimited(Tablut &__currentMove, int __qDepth, int __alpha, int __beta, int __color)
    {

        // QUIESCENCE MAX DEPTH OR GAME OVER CONDITION
        if (_stopWatch.isTimeouted() || __currentMove.isGameOver() || __qDepth == _quiesceMaxDepth)
        {
            return evaluate(__currentMove, __qDepth, __color);
        }

        _qTotalMoves++;

        int score = BOTTOM_SCORE;
        std::vector<Tablut> moves;

        int standPat = evaluate(__currentMove, __qDepth, __color);

        if (standPat >= __beta)
        {
            return __beta;
        }

        __alpha = std::max(__alpha, standPat);

        // GENERATE ALL NON QUIET MOVES ( CAPTURE OR WINS )
        getMoves(__currentMove, moves);

        // NO MOVE AVAILABLE, LOSE BY NO MOVE LEFT
        if (moves.size() == 0)
        {
            if (__color)
            {
                __currentMove._gameState = GAME_STATE::BLACKWIN;
            }
            else
            {
                __currentMove._gameState = GAME_STATE::WHITEWIN;
            }

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

        // NO NON-QUIET MOVE AVAILABLE, QUIET STATE SO WE RETURN NORMAL HEURISTIC VALUE
        if (score == BOTTOM_SCORE)
        {
            return evaluate(__currentMove, __qDepth, __color);
        }

        return __alpha;
    }

    int NegaScout(Tablut &__currentMove, const int __depth, int __alpha, int __beta, const bool __color)
    {
        _totalMoves++;

        int score;
        int b;
        int v;

        std::vector<Tablut> moves;
        Tablut move;

        score = BOTTOM_SCORE;
        b = __beta;

        if (__currentMove.isGameOver() || __depth == 0)
        {
            return Quiesce(__currentMove, __depth, __alpha, __beta, __color);
        }

        getMoves(__currentMove, moves);

        // ADD HASH TO CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
        addHashToMoves(moves);

        // LOSE BY NO MOVE LEFT
        if (moves.size() == 0)
        {
            if (__color)
            {
                __currentMove._gameState = GAME_STATE::BLACKWIN;
            }
            else
            {
                __currentMove._gameState = GAME_STATE::WHITEWIN;
            }

            return Quiesce(__currentMove, __depth, __alpha, __beta, __color);
        }

        // SORT MOVES
        sortMoves(moves, __depth, __color);

        // NEGASCOUT CORE ENGINE
        for (int i = 0; i < moves.size(); i++)
        {
            move = moves[i];

            v = -NegaScout(move, __depth - 1, -b, -__alpha, !__color);

            if (v > __alpha && v < __beta && i > 0)
            {
                v = -NegaScout(move, __depth - 1, -__beta, -__alpha, !__color);
            }

            if (v > score)
            {
                if (__depth == _maxDepth)
                {
                    _bestScore = v;
                    _bestMove = move;
                }
                score = v;
            }

            __alpha = std::max(__alpha, v);

            if (__alpha >= __beta)
            {
                storeKillerMove(move, __depth);
                _cutOffs[__depth]++;
                break;
            }

            b = __alpha + 1;
        }

        return score;
    }

    int NegaScoutTT(Tablut &__currentMove, const int __depth, int __alpha, int __beta, const bool __color)
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
            tt_entry = maybe_entry.value();
            int tt_depth = std::get<ENTRY::DEPTH_INDEX>(tt_entry);

            if (tt_depth >= __depth)
            {
                _transpositionTable.cacheHit();

                FLAG tt_entry_flag = std::get<ENTRY::FLAG_INDEX>(tt_entry);

                // CHECK IF ENTRY HAS BEEN PUT BY THE SAME COLOR, IF COLOR IS NOT THE SAME THEN NEGATE THE SCORE
                int tt_score = std::get<ENTRY::COLOR_INDEX>(tt_entry) == __color ? std::get<ENTRY::SCORE_INDEX>(tt_entry) : -std::get<ENTRY::SCORE_INDEX>(tt_entry);

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
                    storeKillerMove(__currentMove, __depth);
                    _cutOffs[__depth]++;
                    return tt_score;
                }
            }
        }

        // --------TRANSPOSITION TABLE LOOKUP -------- END

        if (__currentMove.isGameOver() || __depth == 0)
        {
            return Quiesce(__currentMove, __depth, __alpha, __beta, __color);
        }

        score = BOTTOM_SCORE;
        b = __beta;

        getMoves(__currentMove, moves);

        // ADD HASH TO CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
        addHashToMoves(moves);

        // LOSE BY NO MOVE LEFT
        if (moves.size() == 0)
        {
            if (__color)
            {
                __currentMove._gameState = GAME_STATE::BLACKWIN;
            }
            else
            {
                __currentMove._gameState = GAME_STATE::WHITEWIN;
            }

            return Quiesce(__currentMove, __depth, __alpha, __beta, __color);
        }

        // SORT MOVES
        sortMoves(moves, __depth, __color);

        // NEGASCOUT CORE ENGINE
        for (int i = 0; i < moves.size(); i++)
        {
            move = moves[i];

            v = -NegaScoutTT(move, __depth - 1, -b, -__alpha, !__color);

            if (v > __alpha && v < __beta && i > 0)
            {
                v = -NegaScoutTT(move, __depth - 1, -__beta, -__alpha, !__color);
            }

            if (v > score)
            {
                if (__depth == _maxDepth)
                {
                    _bestScore = v;
                    _bestMove = move;
                }
                score = v;
            }

            __alpha = std::max(__alpha, v);

            if (__alpha >= __beta)
            {
                storeKillerMove(move, __depth);
                _cutOffs[__depth]++;
                break;
            }

            b = __alpha + 1;
        }

        // -------- TRANSPOSITION TABLE PUT --------

        if (score <= alphaOrigin)
        {
            tt_entry = std::make_tuple(score, __depth, FLAG::UPPERBOUND, __color);
        }
        else if (score >= __beta)
        {
            tt_entry = std::make_tuple(score, __depth, FLAG::LOWERBOUND, __color);
        }
        else
        {
            tt_entry = std::make_tuple(score, __depth, FLAG::EXACT, __color);
        }

        _transpositionTable.put(tt_entry, hash);
        _transpositionTable.cachePut();

        // -------- TRANSPOSITION TABLE PUT -------- END

        return score;
    }

    int NegaScoutTimeLimited(Tablut &__currentMove, const int __depth, int __alpha, int __beta, const bool __color)
    {
        _totalMoves++;

        int score;
        int b;
        int v;

        std::vector<Tablut> moves;
        Tablut move;

        score = BOTTOM_SCORE;
        b = __beta;

        if (_stopWatch.isTimeouted() || __currentMove.isGameOver() || __depth == 0)
        {
            return QuiesceTimeLimited(__currentMove, __depth, __alpha, __beta, __color);
        }

        getMoves(__currentMove, moves);

        // ADD HASH TO CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
        addHashToMoves(moves);

        // LOSE BY NO MOVE LEFT
        if (moves.size() == 0)
        {
            if (__color)
            {
                __currentMove._gameState = GAME_STATE::BLACKWIN;
            }
            else
            {
                __currentMove._gameState = GAME_STATE::WHITEWIN;
            }

            return QuiesceTimeLimited(__currentMove, __depth, __alpha, __beta, __color);
        }

        // SORT MOVES
        sortMoves(moves, __depth, __color);

        // NEGASCOUT CORE ENGINE
        for (int i = 0; i < moves.size(); i++)
        {
            move = moves[i];

            v = -NegaScoutTimeLimited(move, __depth - 1, -b, -__alpha, !__color);

            if (v > __alpha && v < __beta && i > 0)
            {
                v = -NegaScoutTimeLimited(move, __depth - 1, -__beta, -__alpha, !__color);
            }

            if (v > score)
            {
               
                if (__depth == _maxDepth)
                {
                    _bestScore = v;
                    _bestMove = move;
                }
                score = v;
            }

            __alpha = std::max(__alpha, v);

            if (__alpha >= __beta)
            {
                storeKillerMove(move, __depth);
                _cutOffs[__depth]++;
                break;
            }

            b = __alpha + 1;
        }

        return score;
    }
};

// NEGASCOUT __________________________________________
/*


    Tablut TimeLimitedSearch(Tablut &__startingPosition, StopWatch &_globalTimer, int __threads = MAX_THREADS)
    {
        reset();

        std::vector<Tablut> moves;
        std::vector<std::future<int>> results;

        const bool color = __startingPosition._isWhiteTurn;

        const int originalDepth = _maxDepth;

        _bestScore = BOTTOM_SCORE;
        int v;

        // GENERATE ALL LEGAL MOVES
        getMoves(__startingPosition, moves);

        // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
        addHashToMoves(moves);

        // SORT MOVES
        sortMoves(moves, _maxDepth, color);

        // TIME LIMIT SUBDIVISION
        int totalMoves = moves.size();

        for (int t = 0; t < moves.size(); t += __threads)
        {
            // DIVIDE TIME BY GROUP OF THREADS, APPLY A TOLERANCE OF N% FOR EVERY GROUP OF THREAD;
            _computeSliceTimeLimit(_globalTimer, _stopWatch, totalMoves, __threads);
            _stopWatch.start();

            for (int i = 0; i < __threads && i + t < moves.size(); i++)
            {
                results.push_back(std::async(std::launch::async, &NegaScoutEngine::NegaScoutTimeLimited, this, std::ref(moves[i + t]), _maxDepth - 1, BOTTOM_SCORE, TOP_SCORE, !color));
            }

            for (int i = 0; i < results.size(); i++)
            {
                v = -results[i].get();

                if (v > _bestScore || t + i == 0)
                {
                    _bestMove = moves[i + t];
                    _bestScore = v;
                }
            }

            totalMoves -= __threads;

            results.clear();
            _stopWatch.reset();
        }

        _maxDepth = originalDepth;

        return _bestMove;
    }

*/
#endif
