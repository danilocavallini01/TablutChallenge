#ifndef FITNESS
#define FITNESS

#include "../Model/Tablut.hpp"
#include "../Model/MoveGenerator.hpp"
#include "../Model/Heuristic.hpp"
#include "../Model/Engine/NegaScoutEngine.hpp"

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
    void static printStats(NegaScoutEngine &__engine, std::chrono::steady_clock::time_point &__timeBegin, std::chrono::steady_clock::time_point &__timeEnd, int __totalTime, int __cicles)
    {
        __engine.print();

        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(__timeEnd - __timeBegin).count() << "[ms]" << std::endl;
        std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(__totalTime) / (__cicles + 1) << "[ms]" << std::endl;
    }
    // Do a single match and register both fitness results in a pair <WHITE,BLACK> fitnesses
    std::pair<double, double> _match(Weights &__white, Weights &__black, int __threadId = -1)
    {
        Tablut gameBoard = Tablut::getStartingPosition();

        // Zobrist hash instance
        Zobrist hasher = Zobrist();

        // Setup new search engine with the given weights heuristic
        NegaScoutEngine searchEngineWhite = NegaScoutEngine(_maxDepth, _maxQDepth, Heuristic(__white), hasher);
        NegaScoutEngine searchEngineBlack = NegaScoutEngine(_maxDepth, _maxQDepth, Heuristic(__black), hasher);

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
            // gameBoard = searchEngineWhite.TimeLimitedSearch(gameBoard, timerWhite);
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
            // gameBoard = searchEngineBlack.TimeLimitedSearch(gameBoard, timerBlack);
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

            /*
             // USER DEFINED PLAY ---------
            Pos fromX, fromY, toX, toY;

            std::cout << "Insert [from X] [from Y] [to X] [to Y]: ";
            std::cin >> fromX;
            std::cin >> fromY;
            std::cin >> toX;
            std::cin >> toY;

            gameBoard = gameBoard.next(fromX, fromY, toX, toY);

            // GAME CICLE
            gameBoard.print();

            // CHECK GAME_STATE
            if (_checkWin(gameBoard))
                break;

            */
        }

        int gameCicles = i + 1;

        double whiteFitness = _computeFitness(searchEngineWhite, double(totalWhiteScore) / gameCicles, double(totalTimeWhite) / gameCicles, gameBoard, gameCicles, true);
        double blackFitness = _computeFitness(searchEngineBlack, double(-totalBlackScore) / gameCicles, double(totalTimeBlack) / gameCicles, gameBoard, gameCicles, false);

        return std::make_pair(whiteFitness, blackFitness);
    }

    /*
        Compute fitness for a specific game for both black and white player
        I compute fitness by this parameters of a player:
            - total moves checked ( less is better )
            - avg moves score by player ( more is better )
            - avg time elapsed to play a move ( less is better )
            - if as won or not:
                - WIN: fitness returned without changes
                - LOSE: fitness returned with negative sign
                - DRAW: fitness returned halved
    */
    double _computeFitness(NegaScoutEngine &__engine, double __avgScore, double __avgTimeElapsed, Tablut __gameBoard, int gameCicles, bool __isWhite)
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