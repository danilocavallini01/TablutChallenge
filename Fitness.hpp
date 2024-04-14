#ifndef FITNESS
#define FITNESS

#include "Tablut.h"
#include "MoveGenerator.h"
#include "Heuristic.hpp"
#include "SearchEngine.h"

#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

class Fitness
{
private:
    int _maxDepth = 7;
    int _maxIterations = 250;

public:
    Fitness() : _maxDepth(7), _maxIterations(250){};
    Fitness(int __maxDepth, int __maxIterations = 250) : _maxDepth(__maxDepth), _maxIterations(__maxIterations){};
    ~Fitness(){};

private:
    // Do a single match and register both fitness results in a pair <WHITE,BLACK> fitnesses
    std::pair<double, double> _match(Weights &__white, Weights & __black)
    {
        Tablut gameBoard = Tablut::getStartingPosition();

        // Setup new search engine with the given weights heuristic
        SearchEngine searchEngineWhite = SearchEngine(Heuristic(__white));
        SearchEngine searchEngineBlack = SearchEngine(Heuristic(__black));

        int totalWhiteScore = 0;
        int totalBlackScore = 0;

        int64_t totalTimeWhite = 0;
        int64_t totalTimeBlack = 0;

        std::chrono::steady_clock::time_point timeBegin;
        std::chrono::steady_clock::time_point timeEnd;

        int i;

        for (i = 0; i < _maxIterations / 2; i++)
        {
            // WHITE ONE --------------------
            // NEGASCOUT --------------------
            timeBegin = std::chrono::steady_clock::now();
            gameBoard = searchEngineWhite.NegaScoutSearch(gameBoard, _maxDepth);
            timeEnd = std::chrono::steady_clock::now();

            // GAME CICLE
            gameBoard.print();

            std::cout << " --> NEGASCOUT SCORE = " << searchEngineWhite._bestScore << std::endl;
            totalWhiteScore += searchEngineWhite._bestScore;

            // PERFORMANCE _______________
            std::cout << searchEngineWhite._transpositionTable << std::endl;

            std::cout << "TOTAL MOVES CHECKED: " << searchEngineWhite.getTotalMoves() << std::endl;
            totalTimeWhite += std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count();
            std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count() << "[ms]" << std::endl;
            std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(totalTimeWhite) / (i + 1) << "[ms]" << std::endl;

            searchEngineWhite._transpositionTable.resetStat();

            // CHECK GAME_STATE
            if (_checkWin(gameBoard))
            {
                break;
            }

            // BLACK ONE --------------------
            // NEGASCOUT --------------------
            timeBegin = std::chrono::steady_clock::now();
            gameBoard = searchEngineBlack.NegaScoutSearch(gameBoard, _maxDepth);
            timeEnd = std::chrono::steady_clock::now();

            // GAME CICLE
            gameBoard.print();

            std::cout << " --> NEGASCOUT SCORE = " << searchEngineBlack._bestScore << std::endl;
            totalBlackScore += searchEngineBlack._bestScore;

            // PERFORMANCE _______________
            std::cout << searchEngineBlack._transpositionTable << std::endl;

            std::cout << "TOTAL MOVES CHECKED: " << searchEngineBlack.getTotalMoves() << std::endl;
            totalTimeBlack += std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count();
            std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count() << "[ms]" << std::endl;
            std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(totalTimeBlack) / (i + 1) << "[ms]" << std::endl;

            searchEngineBlack._transpositionTable.resetStat();

            // CHECK GAME_STATE
            if (_checkWin(gameBoard))
            {
                break;
            }

            std::cout << "- NEW ROUND ------------------------" << std::endl;
            std::cout << "-----------------------------------" << std::endl;
        }

        int gameCicles = i + 1;

        double whiteFitness = _computeFitness(searchEngineWhite, double(totalWhiteScore) / gameCicles, double(totalTimeBlack) / gameCicles, gameBoard._gameState, true);
        double blackFitness = _computeFitness(searchEngineBlack, double(totalBlackScore) / gameCicles, double(totalTimeBlack) / gameCicles, gameBoard._gameState, false);

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
    double _computeFitness(SearchEngine __engine, double __avgScore, double __avgTimeElapsed, GAME_STATE __gameState, bool __isWhite)
    {
        double fitness = (1.0 / double(__engine.getTotalMoves())) * (__avgScore) * (1.0 / __avgTimeElapsed) * 10000.0;

        if (__isWhite)
        {
            if (__gameState == GAME_STATE::WHITEWIN)
            {
                return fitness;
            }
            else if (__gameState == GAME_STATE::BLACKDRAW || __gameState == GAME_STATE::WHITEDRAW)
            {
                return fitness - 2000.0;
            }
            else
            {
                return fitness - 200000.0;
            }
        }
        else
        {
            if (__gameState == GAME_STATE::BLACKWIN)
            {
                return fitness;
            }
            else if (__gameState == GAME_STATE::BLACKDRAW || __gameState == GAME_STATE::WHITEDRAW)
            {
                return fitness - 2000.0;
            }
            else
            {
                return fitness - 200000.0;
            }
        }
    }

    // CHECK IF A GAME STATE IS IN A WIN POSITION
    bool _checkWin(Tablut &__position)
    {
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

        for (int i = 0; i < __totalMatches; i++)
        {
            results.push_back(_match(__whitePopulation[i], __blackPopulation[i]));
        }

        return results;
    }
};

#endif