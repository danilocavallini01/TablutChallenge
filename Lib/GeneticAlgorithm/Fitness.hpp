#ifndef FITNESS
#define FITNESS

#include "Lib/Tablut/Tablut.hpp"
#include "Lib/Tablut/MoveGenerator.hpp"
#include "Lib/Tablut/Heuristic.hpp"
#include "Lib/Tablut/TablutNegaScoutEngine.hpp"

#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

const int DEFAULT_MAX_DEPTH = 7;
const int DEFAULT_MAX_QDEPTH = 2;

using namespace AI::Engine;

class Fitness
{
private:
    int _maxDepth;
    int _maxQDepth;
    int _maxIterations;
    bool _verbose;

public:
    Fitness(int __maxDepth = DEFAULT_MAX_DEPTH, int __maxQDepth = DEFAULT_MAX_QDEPTH, int __maxIterations = 250, bool __verbose = true) : _maxDepth(__maxDepth),
                                                                                                                                          _maxQDepth(__maxQDepth),
                                                                                                                                          _maxIterations(__maxIterations),
                                                                                                                                          _verbose(__verbose){};
    Fitness(bool __verbose) : Fitness(DEFAULT_MAX_DEPTH, DEFAULT_MAX_QDEPTH, 250, __verbose){};
    ~Fitness(){};

private:
    void static printStats(TNegaScoutEngine &__engine, std::chrono::steady_clock::time_point &__timeBegin, std::chrono::steady_clock::time_point &__timeEnd, int __totalTime, int __cicles)
    {
        __engine.print();

        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(__timeEnd - __timeBegin).count() << "[ms]" << std::endl;
        std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(__totalTime) / (__cicles + 1) << "[ms]" << std::endl;
    }

    /**
     * @brief do a single match between a white player described by @param __white Weights and a black one described by @param __black Weights
     * at the end of the match call @fn _computeFitness by passing various stats stored during the game to it
     * 
     * @param __white - white player Weights 
     * @param __black - black player Weights
     * @param __threadId - thread id for debugging purpose, if id is -1 then match is not running on a separate thread
     * @return std::pair<double, double> - white and black fitness
     */
    std::pair<double, double> _match(Weights &__white, Weights &__black, int __threadId = -1)
    {
        Tablut gameBoard = Tablut::getStartingPosition();

        // Zobrist hash instance
        Zobrist hasher = Zobrist();

        // Setup new search engine with the given weights heuristic
        TNegaScoutEngine searchEngineWhite = TNegaScoutEngine(_maxDepth, _maxQDepth, Heuristic(__white), hasher);
        TNegaScoutEngine searchEngineBlack = TNegaScoutEngine(_maxDepth, _maxQDepth, Heuristic(__black), hasher);

        for (int n : __white)
            std::cout << n << ' ';
        std::cout << std::endl;

        for (int n : __black)
            std::cout << n << ' ';
        std::cout << std::endl;

        int totalWhiteScore = 0;
        int totalBlackScore = 0;

        int64_t totalTimeWhite = 0;
        int64_t totalTimeBlack = 0;

        std::chrono::steady_clock::time_point timeBegin;
        std::chrono::steady_clock::time_point timeEnd;

        StopWatch timerWhite = StopWatch(60000);
        StopWatch timerBlack = StopWatch(60000);

        int i;

        hasher.addHash(gameBoard);

        for (i = 0; i < _maxIterations / 2; i++)
        {
            // WHITE ONE --------------------
            // NEGASCOUT --------------------

            timerWhite.start();
            timeBegin = std::chrono::steady_clock::now();
            gameBoard = searchEngineWhite.Search(gameBoard);
            timeEnd = std::chrono::steady_clock::now();
            timerWhite.reset();

            // STAT ------------------

            totalWhiteScore += searchEngineWhite.getBestScore();
            totalTimeWhite += std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count();

            if (_verbose)
            {
                gameBoard.print();
                printStats(searchEngineWhite, timeBegin, timeEnd, totalTimeWhite, i);
            }

            // CHECK GAME_STATE
            if (_checkWin(gameBoard))
            {
                break;
            }

            // BLACK ONE --------------------
            // NEGASCOUT --------------------

            timerBlack.start();
            timeBegin = std::chrono::steady_clock::now();
            gameBoard = searchEngineBlack.Search(gameBoard);
            timeEnd = std::chrono::steady_clock::now();
            timerBlack.reset();

            // STAT ------------------

            totalBlackScore += searchEngineBlack.getBestScore();
            totalTimeBlack += std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count();

            if (_verbose)
            {
                gameBoard.print();
                printStats(searchEngineBlack, timeBegin, timeEnd, totalTimeBlack, i);
            }

            // CHECK GAME_STATE
            if (_checkWin(gameBoard))
            {
                break;
            }

            if (_verbose)
            {
                std::cout << "- NEW ROUND ------------------------" << std::endl;
                std::cout << "-----------------------------------" << std::endl;
            }


        }

        int gameCicles = i + 1;

        double whiteFitness = _computeFitness(searchEngineWhite, double(totalWhiteScore) / gameCicles, double(totalTimeWhite) / gameCicles, gameBoard, gameCicles, true);
        double blackFitness = _computeFitness(searchEngineBlack, double(-totalBlackScore) / gameCicles, double(totalTimeBlack) / gameCicles, gameBoard, gameCicles, false);

        return std::make_pair(whiteFitness, blackFitness);
    }

    
    /**
     * @brief Compute fitness for a specific game for both black and white player
            I compute fitness by this parameters of a player:
                - avg time elapsed to compute a single moves ( less is better )
                - total turn taken to reach the end of game ( less is better )
                - total allied - enemy cells remaining at the end of the game ( higher is better )

                - if as won or not:
                    - WIN: fitness returned without changes
                    - LOSE: fitness returned - 200000 points
                    - DRAW: fitness returned - 100000 points
     * 
     * @param __engine - search engine used to evaluate the game
     * @param __avgScore - avg player score
     * @param __avgTimeElapsed - avg time used to compute a move
     * @param __gameBoard - ending board
     * @param gameCicles - total number of turn
     * @param __isWhite - is white player or not
     * @return double - FITNESS
     */
    double _computeFitness(TNegaScoutEngine &__engine, double __avgScore, double __avgTimeElapsed, Tablut __gameBoard, int gameCicles, bool __isWhite)
    {
        double scoreWeight = 0.1;
        double avgTimeWeight = 10.0;
        double gameCicleWeight = 10.0;
        double cutOffFitness = 0.0;

        double fitness = 0.0;
        // fitness += (__avgScore) * scoreWeight;
        fitness += (1000.0 / __avgTimeElapsed) * avgTimeWeight;
        fitness += (100.0 / double(gameCicles)) * gameCicleWeight;

        /*
            for (int i = 0; i < MAX_DEFAULT_DEPTH; i++)
            {
                cutOffFitness += __engine.getCutOffs(i) * pow(10, i);
            }

            cutOffFitness = std::log10(cutOffFitness);
    */
        if (__isWhite)
        {
            fitness += __gameBoard._whiteCount * 20.0 - __gameBoard._blackCount * 10.0;
        }
        else
        {
            fitness += -__gameBoard._whiteCount * 20.0 + __gameBoard._blackCount * 10.0;
        }

        // WIN FITNESS
        if (__isWhite)
        {
            if (__gameBoard._gameState == GAME_STATE::WHITEWIN)
            {
                return fitness;
            }
        }
        else
        {
            if (__gameBoard._gameState == GAME_STATE::BLACKWIN)
            {
                return fitness;
            }
        }

        // DRAW FITNESS
        if (__gameBoard._gameState == GAME_STATE::BLACKDRAW || __gameBoard._gameState == GAME_STATE::WHITEDRAW)
        {
            return fitness - 100000.0;
        }

        // LOSE OR TOO MUCH ITERATIONS FITNESS
        return fitness - 200000.0;
    }

    // CHECK IF A GAME STATE IS IN A WIN POSITION
    bool _checkWin(Tablut &__position)
    {
        __position.checkWinState();
        if (__position.isGameOver())
        {
            std::cout << "########################" << std::endl;
            std::cout << (__position.checkWinState() == GAME_STATE::BLACKWIN ? " BLACK WON " : (__position.checkWinState() == GAME_STATE::WHITEWIN ? " WHITE WON " : " DRAW ")) << std::endl;
            std::cout << "########################" << std::endl;

            return true;
        }
        return false;
    }

public:
    /**
     * @brief train one generation of an ai by doing @param __totalMatches matches (@see fn _match()) between white and black population, every single match is done in 
     * a separate thread to speed up training process
     *
     * @param __whitePopulation - white population to train, vector of Weights used in Heuristic score evaluation
     * @param __blackPopulation - black population ot train, vector of Weights used in Heuristic score evaluation
     * @param __totalMatches - number of matches
     * @return std::vector<std::pair<double, double>> a vector containing respectively (white,black) the computed fitnesses for every match done
     */
    std::vector<std::pair<double, double>> train(std::vector<Weights> &__whitePopulation, std::vector<Weights> &__blackPopulation, int __totalMatches)
    {
        std::vector<std::pair<double, double>> results{};
        std::vector<std::future<std::pair<double, double>>> threads{};

        for (int i = 0; i < __totalMatches; i++)
        {
            threads.push_back(std::async(std::launch::async, &Fitness::_match, std::ref(*this), std::ref(__whitePopulation[i]), std::ref(__blackPopulation[i]), i));
        }

        for (int i = 0; i < __totalMatches; i++)
        {
            results.push_back(threads[i].get());
        }

        return results;
    }
};

#endif