#ifndef GALG
#define GALG

#include "Heuristic.h"

#include <vector>
#include <cstdint>
#include <random>
#include <ctime>

// Forward Declaration
class Heuristic;

class GA
{
private:
    int _N;
    int _offSpring;
    int _genes;
    double _mutationProb;
    int _tournSize;
    int _maxGeneration;
    int _totalWeights;

    std::vector<Weights> _whitePopulation;
    std::vector<Weights> _blackPopulation;

    void initalizePopulation()
    {
        // Random Number Generators initialize
        std::random_device rd;
        std::mt19937 gen(rd());
        std::srand(std::time(nullptr));

        std::uniform_int_distribution<ZobristKey> distribution(1, 1000);

        // Resetting any previous population
        _whitePopulation = {};
        _blackPopulation = {};
        Weights add;

        // Generating random weights from uniform distribution
        for (int i = 0; i < _N * 2; i++)
        {
            add = {};

            for (int j = 0; j < _totalWeights; j++)
            {
                add[j] = distribution(gen);
            }

            // if even add to white population else to the black one
            if (i % 2 == 0)
            {
                _whitePopulation.push_back(add);
            }
            else
            {
                _blackPopulation.push_back(add);
            }
        }
    }
public:
    GA() : _N(10),
           _offSpring(2),
           _genes(11),
           _mutationProb(0.2),
           _tournSize(3),
           _maxGeneration(3),
           _totalWeights(7) {}
    ~GA(){};

    // Start training model
    void start()
    {
        initalizePopulation();
        std::cout << "---------INITIALIZE POPULATION GAME---------" << std::endl;
    }
};

#endif
