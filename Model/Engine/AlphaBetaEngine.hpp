#ifndef ALPHABETA_SEARCH_ENGINE
#define ALPHABETA_SEARCH_ENGINE


#include "../../Abstract/Engine/AbstractAlphaBetaEngine.hpp"

#include "../../Abstract/TranspositionTable.hpp"
#include "../Heuristic.hpp"
#include "../Zobrist.hpp"

using namespace AI::Abstract;
using namespace AI::Abstract::Engine;
using namespace AI::Abstract::Define;
using namespace AI::Interface::Engine;

namespace AI
{
    namespace Engine
    {
        class AlphaBetaEngine : public AbstractAlphaBetaEngine<Tablut, Heuristic, Zobrist, TranspositionTable<Entry>>
        {
        public:
            AlphaBetaEngine(int __maxDepth, int __quiescenceMaxDepth, Heuristic __heuristic, Zobrist __zobrist, TranspositionTable<Entry> __table = TranspositionTable<Entry>())
                : AbstractAlphaBetaEngine(__maxDepth, __quiescenceMaxDepth, __heuristic, __zobrist, __table)
            {
            }

            ~AlphaBetaEngine(){};

            constexpr AlphaBetaEngine &operator=(const AlphaBetaEngine &__other)
            {
                std::memcpy(this, &__other, sizeof(AlphaBetaEngine));
                return *this;
            }

            // ALPHABETA __________________________________________

            Tablut Search(Tablut &__startingPosition) override
            {
                reset();
                resetTranspositionTable();

                AlphaBeta(__startingPosition, _maxDepth, BOTTOM_SCORE, TOP_SCORE, __startingPosition._isWhiteTurn);

                return _bestMove;
            }

            int AlphaBeta(Tablut &__currentMove, const int __depth, int __alpha, int __beta, bool __color)
            {
                _totalMoves++;

                int score;

                std::vector<Tablut> moves;
                Tablut move;

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
                    if (__currentMove._isWhiteTurn)
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

                // ALPHA BETA ENGINE CORE
                if (__color)
                {
                    // MAXIMIZE PLAYER
                    score = BOTTOM_SCORE;

                    for (int i = 0; i < moves.size(); i++)
                    {
                        score = std::max(score, AlphaBeta(moves[i], __depth - 1, __alpha, __beta, !__color));

                        if (score > __alpha)
                        {
                            __alpha = score;
                            if (__depth == _maxDepth)
                            {
                                _bestScore = score;
                                _bestMove = moves[i];
                            }
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
                    // MINIMIZE
                    score = TOP_SCORE;

                    for (int i = 0; i < moves.size(); i++)
                    {
                        score = std::min(score, AlphaBeta(moves[i], __depth - 1, __alpha, __beta, !__color));

                        if (score < __beta)
                        {
                            __beta = score;
                            if (__depth == _maxDepth)
                            {
                                _bestScore = score;
                                _bestMove = moves[i];
                            }
                        }

                        if (score <= __alpha)
                        {
                            _cutOffs[__depth]++;
                            break;
                        }
                    }
                }

                return score;
            }
        };

    }
}
#endif