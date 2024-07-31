#ifndef ABSTRACT_SEARCH_ENGINE
#define ABSTRACT_SEARCH_ENGINE

#include "Lib/Interfaces/IEngine.hpp"
#include "Lib/Interfaces/IHeuristic.hpp"
#include "Lib/Interfaces/IZobrist.hpp"

#include "TranspositionTable.hpp"

#include <thread>
#include <iostream>
#include <chrono>
#include <cstring>
#include <future>
#include <atomic>

using namespace std::chrono_literals;
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
         * @tparam MG - Move Generator type
         * @tparam Z - Zobrist Fn type
         * @tparam TT - Transposition Table type
         * @tparam M - Move type
         */
        template <typename G, typename H, typename MG, typename Z, typename TT, typename M = StandardMove>

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

            void _resetCutoffs() override
            {
                for (int i = 0; i < _maxDepth; i++)
                {
                    _cutOffs[i] = 0;
                }
            }

            /**
             * @brief Utility function, given a @see globalTimer slice wisely the time so that the parallel search go in timoeout exactly at the specified time
             * EX. if there is a 60s timer and there are 4 total moves and 2 threads, this will set the @see __mustSetTimer first with 30s - MAX_TIME_ERROR% 
             * 
             * @param __globalTimer - timer given by an external source
             * @param __mustSetTimer - timer that needs the sliced time
             * @param __remainingMoves - total remaining moves
             * @param __threads - total threads used
             */
            void _computeSliceTimeLimit(StopWatch &__globalTimer, StopWatch &__mustSetTimer, int __remainingMoves, int __threads) override
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

            /**
             * @brief evaluate a position at a given depth by passing the state to the heuristic function
             * 
             * @param __move - the game to evaluate
             * @param __depth - current depth of evaluation
             * @param __color - evaluation side
             * @return int - score
             */
            int evaluate(G &__move, int __depth, bool __color) override
            {
                return _heuristic.evaluate(__move, __depth, __color, _colored);
            }

            /**
             * @brief compute hash function of all given moves by using the given hash function and add
             * the hash to the state
             * @see IZobrist
             * 
             * @param __moves - moves that needs hash
             */
            void addHashToMoves(std::vector<G> &__moves) override
            {
                for (auto &nextGameState : __moves)
                {
                    _zobrist.addHash(nextGameState);
                }
            }

            /**
             * @brief sort moves by score and killer moves possibly, by using the heuristic function sort methods
             * @see IHeuristic
             * 
             * @param __moves - moves to be sorted
             * @param __depth - current depth of evaluation
             * @param __color - evaluation side
             */
            void sortMoves(std::vector<G> &__moves, int __depth, bool __color) override
            {
                _heuristic.sortMoves(__moves, __depth, __color, _colored);
            }

            /**
             * @brief get next moves possible starting from the @param __move game state
             * 
             * @param __move - starting position
             * @param __moves - ending positions
             */
            void getMoves(G &__move, std::vector<G> &__moves) override
            {
                MG::generateLegalMoves(__move, __moves);
            }

            /**
             * @brief function used to store the next best moves at a given depth,
             * start by coppying the best moves from the next game state and add the best next moves
             * at the end of alla evaluation the array will contain the list of the next best mves
             * 
             * @param __position 
             * @param __bestMove 
             * @param __depth 
             */
            void storeBestMove(G &__position, G &__bestMove, int __depth)
            {
                __position.copyBestMove(__bestMove);
                __position.storeBestMove(__bestMove, _maxDepth - __depth);
            }

            /**
             * @brief store the killer moves at a specified depth
             * 
             * @param __t 
             * @param __depth 
             */
            void storeKillerMove(G &__t, int __depth) override
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

            /**
             * @brief reset all possible stats, values from tt table and killer moves
             * 
             */
            void reset() override
            {
                resetStats();
                //resetTranspositionTable();
                _heuristic.resetKillerMoves();
            }

            void resetStats() override
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