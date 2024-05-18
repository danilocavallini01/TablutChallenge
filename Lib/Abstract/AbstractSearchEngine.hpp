#ifndef ABSTRACT_SEARCH_ENGINE
#define ABSTRACT_SEARCH_ENGINE

#include "../Interfaces/IEngine.hpp"
#include "../Interfaces/IHeuristic.hpp"
#include "../Interfaces/IZobrist.hpp"
#include "TranspositionTable.hpp"

#include "../Model/MoveGenerator.hpp"

#include <thread>
#include <iostream>
#include <cstring>
#include <future>
#include <atomic>

// Forward Declaration
class MoveGenerator;

using namespace AI::Define;
using namespace AI::Interface;

namespace AI
{
    namespace Abstract
    {
        namespace Define
        {
            // Lowest and Highest scores for alpha and beta initialization
            const int BOTTOM_SCORE(-10000000);
            const int TOP_SCORE(10000000);

            // Max possible error accepted by time limited search algorithm ( EXPRESSED AS PERCENTAGE: ES. 20% = 20.0)
            const float MAX_TIME_ERROR = 12.0;

            // Default thread parameter
            const int MAX_THREADS = int(std::thread::hardware_concurrency()) - 1;

        }

        // Total moves
        std::atomic<int> _totalMoves;
        std::atomic<int> _qTotalMoves;

        // Total cutoff made by alpha beta prunings
        std::atomic<int> _cutOffs[20];

        /**
         * @brief Typed params for Abstract engine definition
         *
         * @tparam G - GameState type
         * @tparam H - Heuristic Fn type
         * @tparam Z - Zobrist Fn type
         * @tparam TT - Transposition Table type
         * @tparam M - Move type
         */
        template <typename G, typename H, typename Z, typename TT, typename M = StandardMove>

        class AbstractSearchEngine : public IEngine<G>
        {
        protected:
            H _heuristic;
            TT _transpositionTable;
            Z _zobrist;

            StopWatch _stopWatch;
            StopWatch _globalRemainingTime;

            int _maxDepth;
            int _quiesceMaxDepth;

            int _bestScore;
            G _bestMove;

            bool _colored;

            void _resetCutoffs()
            {
                for (int i = 0; i < _maxDepth; i++)
                {
                    _cutOffs[i] = 0;
                }
            }

            void _computeSliceTimeLimit(StopWatch &__globalTimer, StopWatch &__mustSetTimer, int __remainingMoves, int __threads)
            {
                int slicedTimeLimit = int(float(__globalTimer.getRemainingTime()) / std::ceil(float(__remainingMoves) / float(__threads)) * (100.0 - Define::MAX_TIME_ERROR) / 100.0);
                __mustSetTimer.setTimeLimit(slicedTimeLimit);
            }

        public:
            explicit AbstractSearchEngine(int __maxDepth,
                                          int __quiesceMaxDepth,
                                          bool __colored,
                                          H __heuristic,
                                          Z __zobrist,
                                          TT __transpositionTable)
                : _maxDepth(__maxDepth),
                  _quiesceMaxDepth(-__quiesceMaxDepth),
                  _colored(__colored),
                  _heuristic(__heuristic),
                  _zobrist(__zobrist),
                  _transpositionTable(__transpositionTable)

            {
            }

            virtual ~AbstractSearchEngine(){};

            int evaluate(G &__move, int __depth, bool __color)
            {
                return _heuristic.evaluate(__move, __depth, __color, _colored);
            }

            void addHashToMoves(std::vector<G> &__moves)
            {
                for (auto &nextGameState : __moves)
                {
                    _zobrist.addHash(nextGameState);
                }
            }

            void sortMoves(std::vector<G> &__moves, int __depth, bool __color)
            {
                _heuristic.sortMoves(__moves, __depth, __color, _colored);
            }

            void getMoves(G &__move, std::vector<G> &__moves)
            {
                MoveGenerator::generateLegalMoves(__move, __moves);
            }

            void storeBestMove(G &__position, G &__bestMove, int __depth)
            {
                __position.copyBestMove(__bestMove);
                __position.storeBestMove(__bestMove, _maxDepth - __depth);
            }

            void storeKillerMove(G &__t, int __depth)
            {
                M move = __t.getMove();
                _heuristic.storeKillerMove(move, __depth);
            }

            friend std::ostream &operator<<(std::ostream &out, const AbstractSearchEngine &__engine)
            {
                out << " --> ENGINE SCORE = " << __engine._bestScore << std::endl;

                // PERFORMANCE _______________
                out << __engine._transpositionTable << std::endl;

                out << "TOTAL MOVES CHECKED: " << __engine.getTotalMoves() << ",Q: " << __engine.getQTotalMoves() << std::endl;
                out << "TOTAL CUTOFFS ";

                for (int i = 1; i < __engine._maxDepth; i++)
                {
                    out << "D[" << i << "]:" << __engine.getCutOffs(i) << ",";
                }

                out << std::endl;

                out << "TOTAL KILLER MOVES HIT: " << __engine._heuristic.getKillerMovesHits() << std::endl;

                return out;
            }

            void print()
            {
                std::cout << *this << std::endl;
            }

            void printHeuristic()
            {
                _heuristic.print();
            }

            int getBestScore() const
            {
                return _bestScore;
            }

            int getTotalMoves() const
            {
                return _totalMoves;
            }

            int getQTotalMoves() const
            {
                return _qTotalMoves;
            }

            int getCutOffs(int __index) const
            {
                return _cutOffs[__index];
            }

            void resetTranspositionTable()
            {
                _transpositionTable.clear();
            }

            void reset()
            {
                resetStats();
                // resetTranspositionTable();
                _heuristic.resetKillerMoves();
            }

            void resetStats()
            {
                _totalMoves = 0;
                _qTotalMoves = 0;
                _resetCutoffs();
                _transpositionTable.resetStat();
            }
        };

    }

}

#endif