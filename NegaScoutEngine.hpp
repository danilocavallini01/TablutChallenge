#ifndef NEGASCOUT_SEARCH_ENGINE
#define NEGASCOUT_SEARCH_ENGINE

#include "AbstractSearchEngine.hpp"

class NegaScoutEngine : public AbstractSearchEngine
{
public:
    NegaScoutEngine(Heuristic __heuristic, Zobrist __zobrist, int __maxDepth, int __quiescenceMaxDepth = MAX_DEFAULT_QSEARCH_DEPTH)
        : AbstractSearchEngine(__maxDepth, __quiescenceMaxDepth, true, __heuristic, __zobrist)
    {
    }

    ~NegaScoutEngine(){};

    // NEGASCOUT __________________________________________
    Tablut Search(Tablut &__startingPosition) override
    {
        resetStats();

        NegaScout(__startingPosition, _maxDepth, BOTTOM_SCORE, TOP_SCORE, __startingPosition._isWhiteTurn);

        return _bestMove;
    }

    Tablut ParallelSearch(Tablut &__startingPosition, int __threads = MAX_THREADS)
    {
        resetStats();

        std::vector<Tablut> moves;
        std::vector<std::future<int>> results;

        const bool color = __startingPosition._isWhiteTurn;

        _bestScore = BOTTOM_SCORE;
        int alpha = BOTTOM_SCORE;
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
                results.push_back(std::async(std::launch::async, &NegaScoutEngine::NegaScout, this, std::ref(moves[i + t]), _maxDepth - 1, BOTTOM_SCORE , -alpha, !color));
            }

            for (int i = 0; i < results.size(); i++)
            {
                v = -results[i].get();

                if (v > _bestScore || t + i == 0)
                {
                    _bestMove = moves[i + t];
                    _bestScore = v;

                    alpha = std::max(alpha, v);
                }
            }

            results.clear();
        }

        return _bestMove;
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

            return evaluate(__currentMove, __depth, __color);
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
};

// NEGASCOUT __________________________________________
/*

    int NegaScoutTimeLimited(Tablut &__currentMove, const int __depth, int __alpha, int __beta)
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

            return _heuristic.evaluate(__currentMove, true);
        }

        // SORT MOVES
        _heuristic.sortMoves(moves, true);

        // NEGASCOUT CORE ENGINE
        for (int i = 0; i < moves.size(); i++)
        {
            move = moves[i];
            v = -NegaScoutTimeLimited(move, __depth - 1, -b, -__alpha);

            if (v > __alpha && v < __beta && i > 0)
            {
                v = -NegaScoutTimeLimited(move, __depth - 1, -__beta, -v);
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

    Tablut NegaScoutSearchT(Tablut &__startingPosition, const int __maxDepth = _maxDepth, const int __threads)
    {
        _maxDepth = __maxDepth;
        _totalMoves = 0;
        _resetCutoffs();

        ZobristKey hash;

        std::vector<Tablut> moves;
        Tablut bestMove;
        std::vector<std::future<int>> results;

        const bool color = __startingPosition._isWhiteTurn;

        _bestScore = BOTTOM_SCORE;

        int v = BOTTOM_SCORE;

        // GENERATE ALL LEGAL MOVES
        _moveGenerator.generateLegalMoves(__startingPosition, moves);

        // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
        for (auto &nextTablut : moves)
        {
            _zobrist.addHash(nextTablut);
        }

        // SORT MOVES
        _heuristic.sortMoves(moves, _maxDepth, color, true);

        for (int t = 0; t < moves.size(); t += __threads)
        {
            for (int i = 0; i < __threads && i + t < moves.size(); i++)
            {
                results.push_back(std::async(std::launch::async, &NegaScout, this, std::ref(moves[i + t]), __maxDepth - 1, BOTTOM_SCORE, TOP_SCORE, !color));
            }

            for (int i = 0; i < results.size(); i++)
            {
                v = results[i].get();

                std::cout << v << std::endl;

                if (color)
                {
                    if (v < _bestScore || t + i == 0)
                    {
                        bestMove = moves[i + t];
                        _bestScore = v;
                    }
                }
                else
                {
                    if (v > _bestScore || t + i == 0)
                    {
                        bestMove = moves[i + t];
                        _bestScore = v;
                    }
                }
            }

            results.clear();
        }

        return bestMove;
    }

     Tablut NegaScoutSearchTimeLimited(Tablut &__startingPosition, StopWatch &_globalTimer, const int __threads)
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
        _heuristic.sortMoves(moves, true);

        // TIME LIMIT SUBDIVISION
        totalMoves = moves.size();

        for (int t = 0; t < moves.size(); t += __threads)
        {
            // DIVIDE TIME BY GROUP OF THREADS, APPLY A TOLERANCE OF N% FOR EVERY GROUP OF THREAD;
            _computeSliceTimeLimit(_globalTimer, _stopWatch, totalMoves, __threads);
            _stopWatch.start();

            for (int i = 0; i < __threads && i + t < moves.size(); i++)
            {
                results.push_back(std::async(std::launch::async, &NegaScoutTimeLimited, std::ref(*this), std::ref(moves[i + t]), _maxDepth - 1, BOTTOM_SCORE, TOP_SCORE));
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



    int NegaScoutTT(Tablut &__currentMove, const int __depth, int __alpha, int __beta, const bool __color)
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

                // CHECK IF ENTRY HAS BEEN PUT BY THE SAME COLOR, IF COLOR IS NOT THE SAME THEN NEGATE THE SCORE
                int tt_score = std::get<ENTRY::COLOR_INDEX>(tt_entry) == __color ? std::get<ENTRY::SCORE_INDEX>(tt_entry) : -std::get<ENTRY::SCORE_INDEX>(tt_entry);
                tt_score -= tt_depth - __depth;

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
            return _heuristic.evaluate(__currentMove, __depth, __color, true);
        }

        score = BOTTOM_SCORE;
        b = __beta;

        _moveGenerator.generateLegalMoves(__currentMove, moves);

        // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
        for (auto &nextTablut : moves)
        {
            _zobrist.addHash(nextTablut);
        }

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

            return _heuristic.evaluate(__currentMove, __depth, __color, true);
        }

        // SORT MOVES
        _heuristic.sortMoves(moves, __depth, __color, true);

        // NEGASCOUT CORE ENGINE
        for (int i = 0; i < moves.size(); i++)
        {
            move = moves[i];
            v = -NegaScoutTT(move, __depth - 1, -b, -__alpha, !__color);

            if (v > __alpha && v < __beta && i > 0)
            {
                v = -NegaScoutTT(move, __depth - 1, -__beta, -v, !__color);
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
                _cutOffs[__depth]++;
                break;
            }

            b = __alpha + 1;
        }

        // -------- TRANSPOSITION TABLE PUT --------

        if (score <= alphaOrigin)
        {
            tt_entry = std::make_tuple(score, __depth, FLAG::LOWERBOUND, __color);
        }
        else if (score >= b)
        {
            tt_entry = std::make_tuple(score, __depth, FLAG::UPPERBOUND, __color);
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


*/
#endif