#ifndef NEGASCOUT_SEARCH_ENGINE
#define NEGASCOUT_SEARCH_ENGINE

#include "Lib/Abstract/Engine/AbstractNegaScoutEngine.hpp"

#include "Lib/Abstract/TranspositionTable.hpp"

using namespace AI::Abstract;
using namespace AI::Abstract::Engine;
using namespace AI::Abstract::Define;
using namespace AI::Interface::Engine;

namespace AI
{
    namespace Engine
    {
        std::atomic<int> _alpha;

        template <typename G, typename H, typename MG, typename Z, typename TT = TranspositionTable<Entry>>
        class NegaScoutEngine : public AbstractNegaScoutEngine<G, H, MG, Z, TT>
        {
        public:
            NegaScoutEngine(int __maxDepth, int __quiescenceMaxDepth, H __heuristic, Z __zobrist, TT __table = TT())
                : AbstractNegaScoutEngine<G, H, MG, Z, TT>(__maxDepth, __quiescenceMaxDepth, __heuristic, __zobrist, __table)
            {
            }

            virtual ~NegaScoutEngine(){};

            constexpr NegaScoutEngine &operator=(const NegaScoutEngine &__other)
            {
                std::memcpy(this, &__other, sizeof(NegaScoutEngine));
                return *this;
            }

            // NEGASCOUT __________________________________________
            G Search(G &__startingPosition) override
            {
                this->reset();
                __startingPosition.resetBestMoves();
                this->resetTranspositionTable();

                NegaScout(__startingPosition, this->_maxDepth, BOTTOM_SCORE, TOP_SCORE, __startingPosition._isWhiteTurn);

                return this->_bestMove;
            }

            G ParallelSearch(G &__startingPosition, int __threads = MAX_THREADS) override
            {
                this->reset();
                __startingPosition.resetBestMoves();
                this->resetTranspositionTable();

                std::vector<G> moves;
                std::vector<std::future<int>> results;

                const bool color = __startingPosition._isWhiteTurn;

                this->_bestScore = BOTTOM_SCORE;
                int alpha = BOTTOM_SCORE;
                int beta = TOP_SCORE;
                int b = beta;

                int v;

                // GENERATE ALL LEGAL MOVES
                this->getMoves(__startingPosition, moves);

                // CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
                this->addHashToMoves(moves);

                // SORT MOVES
                this->sortMoves(moves, this->_maxDepth, color);

                for (int t = 0; t < moves.size(); t += __threads)
                {
                    for (int i = 0; i < __threads && i + t < moves.size(); i++)
                    {
                        results.push_back(std::async(std::launch::async, &NegaScoutEngine::NegaScout, this, std::ref(moves[i + t]), this->_maxDepth - 1, -b, -alpha, !color));
                    }

                    for (int i = 0; i < results.size(); i++)
                    {
                        v = -results[i].get();

                        if (v > alpha && v < beta && i + t > 0)
                        {
                            v = -NegaScout(moves[i + t], this->_maxDepth - 1, -beta, -alpha, !color);
                        }

                        if (v > this->_bestScore || t + i == 0)
                        {

                            this->_bestMove = moves[i + t];
                            this->_bestScore = v;
                            this->storeBestMove(__startingPosition, this->_bestMove, this->_maxDepth);
                        }

                        alpha = std::max(alpha, v);

                        b = alpha + 1;
                    }

                    results.clear();
                }

                return this->_bestMove;
            }

            G TimeLimitedSearch(G &__startingPosition, StopWatch &__globalTimer, int __threads = MAX_THREADS) override
            {
                this->reset();
                __startingPosition.resetBestMoves();

                this->_stopWatch = __globalTimer;

                NegaScoutTimeLimited(__startingPosition, this->_maxDepth, BOTTOM_SCORE, TOP_SCORE, __startingPosition._isWhiteTurn);

                return this->_bestMove;
            }

            G TimeLimitedSlicedSearch(G &__startingPosition, StopWatch &__globalTimer, int __threads = MAX_THREADS)
            {
                this->reset();
                __startingPosition.resetBestMoves();

                this->_stopWatch = __globalTimer;

                std::vector<std::future<std::pair<int, G>>> results;
                std::vector<G> moves;
                G move;

                this->_bestScore = BOTTOM_SCORE;

                const bool color = __startingPosition._isWhiteTurn;

                // GET ALL LEGAL MOVES
                this->getMoves(__startingPosition, moves);

                // ADD HASH TO CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
                this->addHashToMoves(moves);

                // SORT MOVES
                this->sortMoves(moves, this->_maxDepth, color);

                _alpha = BOTTOM_SCORE;

                int sliceSize = moves.size() / __threads;
                int i;

                for (i = 0; i < moves.size() - 1 - sliceSize; i += sliceSize + 1)
                {
                    results.push_back(std::async(std::launch::async, &NegaScoutEngine::TimeLimitedSliceSearch, this, std::ref(moves), i, i + sliceSize, this->_maxDepth, color));
                }

                results.push_back(std::async(std::launch::async, &NegaScoutEngine::TimeLimitedSliceSearch, this, std::ref(moves), i, moves.size() - 1, this->_maxDepth, color));

                for (auto &result : results)
                {
                    std::pair<int, G> res = result.get();

                    if (res.first > this->_bestScore)
                    {
                        this->_bestScore = res.first;
                        this->_bestMove = res.second;

                        this->storeBestMove(this->_bestMove, this->_bestMove, this->_maxDepth);
                    }
                }

                return this->_bestMove;
            }

            std::pair<int, G> TimeLimitedSliceSearch(std::vector<G> &__moves, int __startIndex, int __endIndex, int __depth, bool __color)
            {
                _totalMoves++;

                int score = BOTTOM_SCORE;
                int beta = TOP_SCORE;
                int b = beta;
                int v;

                G move;
                G bestMove;

                // NEGASCOUT CORE ENGINE
                for (int i = __startIndex; i < __endIndex; i++)
                {
                    move = __moves[i];

                    v = -NegaScoutTimeLimited(move, __depth - 1, -b, -_alpha, !__color);

                    if (v > _alpha && v < beta && i > 0)
                    {
                        v = -NegaScoutTimeLimited(move, __depth - 1, -beta, -_alpha, !__color);
                    }

                    if (v > score)
                    {
                        bestMove = move;
                        score = v;
                    }

                    _alpha = std::max(_alpha.load(), v);

                    if (_alpha >= beta)
                    {
                        this->storeKillerMove(move, __depth);
                        _cutOffs[__depth]++;
                        break;
                    }

                    b = _alpha + 1;
                }

                return {score, bestMove};
            }

            int Quiesce(G &__currentMove, int __qDepth, int __alpha, int __beta, int __color) override
            {

                // QUIESCENCE MAX DEPTH OR GAME OVER CONDITION
                if (__currentMove.isGameOver() || __qDepth == this->_quiesceMaxDepth)
                {
                    return this->evaluate(__currentMove, __qDepth, __color);
                }

                _qTotalMoves++;

                int score = BOTTOM_SCORE;
                std::vector<G> moves;

                int standPat = this->evaluate(__currentMove, __qDepth, __color);

                if (standPat >= __beta)
                {
                    return __beta;
                }

                __alpha = std::max(__alpha, standPat);

                // GENERATE ALL NON QUIET MOVES ( CAPTURE OR WINS )
                this->getMoves(__currentMove, moves);

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

                    return this->evaluate(__currentMove, __qDepth, __color);
                }

                for (G &move : moves)
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
                    return this->evaluate(__currentMove, __qDepth, __color);
                }

                return __alpha;
            }

            int QuiesceTimeLimited(G &__currentMove, int __qDepth, int __alpha, int __beta, int __color) override
            {

                // QUIESCENCE MAX DEPTH OR GAME OVER CONDITION
                if (this->_stopWatch.isTimeouted() || __currentMove.isGameOver() || __qDepth == this->_quiesceMaxDepth)
                {
                    return this->evaluate(__currentMove, __qDepth, __color);
                }

                _qTotalMoves++;

                int score = BOTTOM_SCORE;
                std::vector<G> moves;

                int standPat = this->evaluate(__currentMove, __qDepth, __color);

                if (standPat >= __beta)
                {
                    return __beta;
                }

                __alpha = std::max(__alpha, standPat);

                // GENERATE ALL NON QUIET MOVES ( CAPTURE OR WINS )
                this->getMoves(__currentMove, moves);

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

                    return this->evaluate(__currentMove, __qDepth, __color);
                }

                for (G &move : moves)
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
                    return this->evaluate(__currentMove, __qDepth, __color);
                }

                return __alpha;
            }

            int NegaScout(G &__currentMove, const int __depth, int __alpha, int __beta, const bool __color)
            {
                _totalMoves++;

                int score;
                int b;
                int v;

                std::vector<G> moves;
                G move;

                score = BOTTOM_SCORE;
                b = __beta;

                if (__currentMove.isGameOver() || __depth == 0)
                {
                    return Quiesce(__currentMove, __depth, __alpha, __beta, __color);
                }

                this->getMoves(__currentMove, moves);

                // ADD HASH TO CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
                this->addHashToMoves(moves);

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
                this->sortMoves(moves, __depth, __color);

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
                        if (__depth == this->_maxDepth)
                        {
                            this->_bestScore = v;
                            this->_bestMove = move;
                        }

                        this->storeBestMove(__currentMove, move, __depth);
                        score = v;
                    }

                    __alpha = std::max(__alpha, v);

                    if (__alpha >= __beta)
                    {
                        this->storeKillerMove(move, __depth);
                        _cutOffs[__depth]++;
                        break;
                    }

                    b = __alpha + 1;
                }

                return score;
            }

            int NegaScoutTT(G &__currentMove, const int __depth, int __alpha, int __beta, const bool __color)
            {
                _totalMoves++;

                const int alphaOrigin = __alpha;
                int score;
                int b;
                int v;

                std::vector<G> moves;
                G move;

                // -------- TRANSPOSITION TABLE LOOKUP --------
                ZobristKey hash = __currentMove._hash;
                std::optional<Entry> maybe_entry = this->_transpositionTable.get(hash);

                Entry tt_entry;

                if (maybe_entry.has_value())
                {
                    tt_entry = maybe_entry.value();
                    int tt_depth = std::get<ENTRY::DEPTH_INDEX>(tt_entry);

                    if (tt_depth >= __depth)
                    {
                        this->_transpositionTable.cacheHit();

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
                            this->storeKillerMove(__currentMove, __depth);
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

                this->getMoves(__currentMove, moves);

                // ADD HASH TO CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
                this->addHashToMoves(moves);

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
                this->sortMoves(moves, __depth, __color);

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
                        if (__depth == this->_maxDepth)
                        {
                            this->_bestScore = v;
                            this->_bestMove = move;
                        }

                        this->storeBestMove(__currentMove, move, __depth);
                        score = v;
                    }

                    __alpha = std::max(__alpha, v);

                    if (__alpha >= __beta)
                    {
                        this->storeKillerMove(move, __depth);
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

                this->_transpositionTable.put(tt_entry, hash);
                this->_transpositionTable.cachePut();

                // -------- TRANSPOSITION TABLE PUT -------- END

                return score;
            }

            int NegaScoutTimeLimited(G &__currentMove, const int __depth, int __alpha, int __beta, const bool __color)
            {
                _totalMoves++;

                int score;
                int b;
                int v;

                std::vector<G> moves;
                G move;

                score = BOTTOM_SCORE;
                b = __beta;

                if (this->_stopWatch.isTimeouted() || __currentMove.isGameOver() || __depth == 0)
                {
                    return QuiesceTimeLimited(__currentMove, __depth, __alpha, __beta, __color);
                }

                this->getMoves(__currentMove, moves);

                // ADD HASH TO CHECK IF MOVE ALREADY DONE(DRAW) AND IF GAME IS IN A WIN OR LOSE POSITION
                this->addHashToMoves(moves);

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
                this->sortMoves(moves, __depth, __color);

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

                        if (__depth == this->_maxDepth)
                        {
                            this->_bestScore = v;
                            this->_bestMove = move;
                        }

                        this->storeBestMove(__currentMove, move, __depth);
                        score = v;
                    }

                    __alpha = std::max(__alpha, v);

                    if (__alpha >= __beta)
                    {
                        this->storeKillerMove(move, __depth);
                        _cutOffs[__depth]++;
                        break;
                    }

                    b = __alpha + 1;
                }

                return score;
            }
        };
    }
}

#endif
