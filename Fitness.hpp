#ifndef FITNESS
#define FITNESS

#include "Tablut.h"
#include "MoveGenerator.h"
#include "Heuristic.h"
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

    int64_t _totalTimeWhite = 0;
    int64_t _totalTimeBlack = 0;

    SearchEngine _searchEngineWhite;
    SearchEngine _searchEngineBlack;

public:
    Fitness() : _maxDepth(7),
                _maxIterations(250),
                _totalTimeWhite(0),
                _totalTimeBlack(0){};
    ~Fitness();

private:
    // Do a single match and register both fitness results in a pair <WHITE,BLACK> fitnesses
    std::pair<int, int> _match(Weights __white, Weights __black)
    {
        int _bestScore;

        Tablut _position = Tablut::getStartingPosition();

        // Setup new search engine with the given weights heuristic
        SearchEngine _searchEngineWhite = SearchEngine(Heuristic(__white));
        SearchEngine _searchEngineBlack = SearchEngine(Heuristic(__black));

        std::chrono::steady_clock::time_point _begin;
        std::chrono::steady_clock::time_point _end;

        for (int i = 0; i < _maxIterations; i++)
        {
            // WHITE ONE --------------------
            // NEGASCOUT --------------------
            _begin = std::chrono::steady_clock::now();
            _position = _searchEngineWhite.NegaScoutSearch(_position, 7);
            _end = std::chrono::steady_clock::now();

            // GAME CICLE
            _position.print();

            std::cout << " --> NEGASCOUT SCORE = " << _searchEngineWhite._bestScore << std::endl;
            // PERFORMANCE _______________
            std::cout << _searchEngineWhite._transpositionTable << std::endl;

            std::cout << "TOTAL MOVES CHECKED: " << _searchEngineWhite.getTotalMoves() << std::endl;
            _totalTimeWhite += std::chrono::duration_cast<std::chrono::milliseconds>(_end - _begin).count();
            std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(_end - _begin).count() << "[ms]" << std::endl;
            std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(_totalTimeWhite) / (i + 1) << "[ms]" << std::endl;

            _searchEngineWhite._transpositionTable.resetStat();

            // CHECK GAME_STATE
            if (_checkWin(_position))
                break;

            // BLACK ONE --------------------
            // NEGASCOUT --------------------
            _begin = std::chrono::steady_clock::now();
            _position = _searchEngineBlack.NegaScoutSearch(_position, 7);
            _end = std::chrono::steady_clock::now();

            // GAME CICLE
            _position.print();

            std::cout << " --> NEGASCOUT SCORE = " << _searchEngineBlack._bestScore << std::endl;
            // PERFORMANCE _______________
            std::cout << _searchEngineBlack._transpositionTable << std::endl;

            std::cout << "TOTAL MOVES CHECKED: " << _searchEngineBlack.getTotalMoves() << std::endl;
            _totalTimeBlack += std::chrono::duration_cast<std::chrono::milliseconds>(_end - _begin).count();
            std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(_end - _begin).count() << "[ms]" << std::endl;
            std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(_totalTimeBlack) / (i + 1) << "[ms]" << std::endl;

            _searchEngineBlack._transpositionTable.resetStat();

            // CHECK GAME_STATE
            if (_checkWin(_position))
                break;

            std::cout << "- NEW ROUND ------------------------" << std::endl;
            std::cout << "-----------------------------------" << std::endl;
        }

        // MUST IMPLEMENT FITNESS FUNCTION
        return std::make_pair(1,1);
    }

    bool _checkWin(Tablut &_position)
    {
        if (_position.isGameOver())
        {
            std::cout << "########################" << std::endl;
            std::cout << (_position.checkWinState() == GAME_STATE::BLACKWIN ? " BLACK WON " : (_position.checkWinState() == GAME_STATE::WHITEWIN ? " WHITE WON " : " DRAW ")) << std::endl;
            std::cout << "########################" << std::endl;

            return true;
        }
        return false;
    }

public:
    std::vector<std::pair<int, int>> fitnessFunction(std::vector<Weights> __whitePopulation, std::vector<Weights> __blackPopulation, int __totalMatches)
    {
        std::vector<std::pair<int, int>> results;

        for (int i = 0; i < __totalMatches; i++)
        {
            results.push_back(_match(__whitePopulation[i], __blackPopulation[i]));
        }

        return results;
    }
};

#endif