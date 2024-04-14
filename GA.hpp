#ifndef GALG
#define GALG

#include "Heuristic.h"
#include "Fitness.hpp"

#include <vector>
#include <cstdint>
#include <random>
#include <ctime>

// Forward Declaration
class Heuristic;
class Fitness;

class GA
{
private:
    int _N;
    double _mutationProb;
    int _mutationFactor;
    int _tournSize;
    int _maxGeneration;
    int _totalWeights;
    Fitness _fitnessFn;

    std::vector<Weights> _whitePopulation;
    std::vector<Weights> _blackPopulation;
    std::random_device _rd;
    std::mt19937 _gen;

    void _initalizePopulation()
    {
        std::uniform_int_distribution<int> distribution(1, 10000);

        // Resetting any previous population
        _whitePopulation = {};
        _blackPopulation = {};

        Weights add;

        // Generating random weights from uniform distribution
        for (int i = 0; i < _N; i++)
        {
            add = {};

            for (int j = 0; j < _totalWeights; j++)
            {
                add[j] = abs((distribution(_gen) % _mutationFactor) - _mutationFactor / 2) * FIXED_WEIGHTS[j];
            }

            _whitePopulation.push_back(add);
        }

        for (int i = 0; i < _N; i++)
        {
            add = {};

            for (int j = 0; j < _totalWeights; j++)
            {
                add[j] = abs((distribution(_gen) % _mutationFactor) - _mutationFactor / 2) * FIXED_WEIGHTS[j];
            }

            _blackPopulation.push_back(add);
        }
    }

    /*
        Use tournament selection algorithm to find the two parents needed for crossover
    */
    void _tournamentSelection(std::pair<Weights, Weights> &__whiteParents, std::pair<Weights, Weights> &__blackParents, std::vector<std::pair<double, double>> &__fitnesses)
    {
        _singleTournamentSelection(__whiteParents.first, __blackParents.first, __fitnesses);
        _singleTournamentSelection(__whiteParents.second, __blackParents.second, __fitnesses);
    }

    /*
        Select next-generation parents after a generation that already player
        Using the tournamentSelection Algorithm ( described below )
        //
            choose k (the tournament size) individuals from the population at random
            choose the best individual from the tournament with probability p
            choose the second best individual with probability p*(1-p)
            choose the third best individual with probability p*((1-p)^2)
            and so on
        //
    */
    void _singleTournamentSelection(Weights &__whiteParent, Weights &__blackParent, std::vector<std::pair<double, double>> &__fitnesses)
    {
        std::uniform_int_distribution<int> distribution(0, _N);

        Weights bestWhitePlayer;
        int bestWhitePlayerScore;

        Weights bestBlackPlayer;
        int bestBlackPlayerScore;

        int selectedWhitePlayer;
        int selectedBlackPlayer;

        for (int i = 0; i < _tournSize; i++)
        {
            // CHOOSE RANDOMLY FROM THE POPULATION
            selectedWhitePlayer = distribution(_gen);
            selectedBlackPlayer = distribution(_gen);

            if (i == 0 || __fitnesses[selectedWhitePlayer].first > bestWhitePlayerScore)
            {
                bestWhitePlayer = _whitePopulation[selectedWhitePlayer];
            }

            if (i == 0 || __fitnesses[selectedBlackPlayer].second > bestBlackPlayerScore)
            {
                bestBlackPlayer = _blackPopulation[selectedBlackPlayer];
            }
        }

        __whiteParent = bestWhitePlayer;
        __blackParent = bestBlackPlayer;
    }

    /*
        In uniform crossover,  each bit is chosen from either parent with equal probability.
        In this, we essentially flip a coin for each chromosome to decide whether or not it will be included in the off-spring.
    */

    void _uniformCrossover(std::pair<Weights, Weights> &__parents, Weights &__offspring1, Weights &__offspring2)
    {
        std::uniform_int_distribution<int> coinToss(0, 1);

        // CROSSOVER START
        for (int i = 0; i < _totalWeights; i++)
        {
            // Toss coin
            if (coinToss(_gen) == 1)
            {
                __offspring1[i] = __parents.first[i];
                __offspring2[i] = __parents.second[i];
            }
            else
            {
                __offspring1[i] = __parents.second[i];
                __offspring2[i] = __parents.first[i];
            }
        }
    }

    /*
        Mutate some chromosome of the offspring with probability -> _mutationProb
        Mutation change the value of the selected chromosome by max +- _mutationFactor / 2
    */
    void _mutate(Weights &__offspring)
    {
        std::uniform_real_distribution<double> mutationProbDistribution(0, 1.0);
        std::uniform_int_distribution<int> mutationChangeDistribution(0, 10000);

        for (int i = 0; i < _totalWeights; i++)
        {
            if (mutationProbDistribution(_gen) <= _mutationProb)
            {
                __offspring[i] += ((mutationChangeDistribution(_gen) % _mutationFactor) - _mutationFactor / 2);
            }
        }
    }

    /*
        Select the best half of new population and best half of the old population to create the new generation
    */
    void _truncationSelection(std::vector<Weights> &__newWhitePopulation, std::vector<Weights> &__newBlackPopulation, std::vector<std::pair<Weights, double>> &__whiteResults, std::vector<std::pair<Weights, double>> &__blackResults)
    {
        // sort by fitness
        std::sort(__whiteResults.begin(), __whiteResults.end(), [](std::pair<Weights, double> __1, std::pair<Weights, double> __2)
                  { return __1.second > __2.second; });
        std::sort(__blackResults.begin(), __blackResults.end(), [](std::pair<Weights, double> __1, std::pair<Weights, double> __2)
                  { return __1.second > __2.second; });

        std::vector<Weights> selectedWhitePopulation{};
        std::vector<Weights> selectedBlackPopulation{};

        for (int i = 0; i < _N / 2; i++)
        {
            selectedWhitePopulation.push_back(__newWhitePopulation.at(i));
            selectedWhitePopulation.push_back(_whitePopulation.at(i));

            selectedBlackPopulation.push_back(__newBlackPopulation.at(i));
            selectedBlackPopulation.push_back(_blackPopulation.at(i));
        }

        _whitePopulation = selectedWhitePopulation;
        _blackPopulation = selectedBlackPopulation;
    }

    void _divideResults(std::vector<std::pair<double, double>> &__results, std::vector<std::pair<Weights, double>> &__whiteResults, std::vector<std::pair<Weights, double>> &__blackResults,
                        std::vector<Weights> &__whitePopulation, std::vector<Weights> &__blackPopulation)
    {
        for (int i = 0; i < __results.size(); i++)
        {
            __whiteResults.push_back({__whitePopulation.at(i), __results.at(i).first});
            __blackResults.push_back({__blackPopulation.at(i), __results.at(i).second});
        }
    }

    void _printGenes()
    {
        std::cout << "---------WHITE POPULATION---------" << std::endl;
        for (int i = 0; i < _whitePopulation.size(); i++)
        {
            std::cout << "WHITE[" << i << "]: ";
            for (int j = 0; j < _totalWeights; j++)
            {
                std::cout << _whitePopulation.at(i)[j] << ",";
            }
            std::cout << std::endl;
        }
        std::cout << "---------BLACK POPULATION---------" << std::endl;
        for (int i = 0; i < _blackPopulation.size(); i++)
        {
            std::cout << "BLACK[" << i << "]: ";
            for (int j = 0; j < _totalWeights; j++)
            {
                std::cout << _blackPopulation.at(i)[j] << ",";
            }
            std::cout << std::endl;
        }
        std::cout << "---------END OF BLACK POPULATION---------" << std::endl;
    }

    void _printFitnesses(std::vector<std::pair<Weights, double>> &__whiteResults, std::vector<std::pair<Weights, double>> &__blackResults)
    {
        std::cout << "---------WHITE POPULATION---------" << std::endl;
        for (int i = 0; i < __whiteResults.size(); i++)
        {
            std::cout << "WHITE[" << i << "]: ";
            for (int j = 0; j < _totalWeights; j++)
            {
                std::cout << __whiteResults.at(i).first[j] << ",";
            }

            std::cout << std::endl;
            std::cout << "FITNESS[" << __whiteResults.at(i).second << "]: ";
            std::cout << std::endl;
        }
        std::cout << "---------BLACK POPULATION---------" << std::endl;
        for (int i = 0; i < __blackResults.size(); i++)
        {
            std::cout << "BLACK[" << i << "]: ";
            for (int j = 0; j < _totalWeights; j++)
            {
                std::cout << __blackResults.at(i).first[j] << ",";
            }

            std::cout << std::endl;
            std::cout << "FITNESS[" << __blackResults.at(i).second << "]: ";
            std::cout << std::endl;
        }
        std::cout << "---------END OF BLACK POPULATION---------" << std::endl;
    }

public:
    GA(int _maxDepth = 7) : _N(10),
                            _mutationProb(0.3),
                            _mutationFactor(100),
                            _tournSize(3),
                            _maxGeneration(10),
                            _totalWeights(TOTAL_WEIGHTS),
                            _fitnessFn(Fitness(_maxDepth))
    {
        std::random_device _rd;
        std::mt19937 _gen(_rd());
        std::srand(std::time(nullptr));
    }

    ~GA(){};

    // Start training model
    void start()
    {
        std::vector<Weights> newWhitePopulation;
        std::vector<Weights> newBlackPopulation;

        std::pair<Weights, Weights> whiteParents;
        std::pair<Weights, Weights> blackParents;

        Weights whiteOffspring1;
        Weights whiteOffspring2;

        Weights blackOffspring1;
        Weights blackOffspring2;

        std::vector<std::pair<Weights, double>> whiteResults;
        std::vector<std::pair<Weights, double>> blackResults;

        std::vector<std::pair<Weights, double>> oldWhiteResults;
        std::vector<std::pair<Weights, double>> oldBlackResults;

        std::vector<std::pair<double, double>> results;
        std::vector<std::pair<double, double>> oldResults;

        std::cout << "---------INITIALIZE POPULATION GAME---------" << std::endl;

        _initalizePopulation();

        _printGenes();
        oldResults = _fitnessFn.train(_whitePopulation, _blackPopulation, _N);

        _divideResults(oldResults, oldWhiteResults, oldBlackResults, _whitePopulation, _blackPopulation);
        _printFitnesses(oldWhiteResults, oldBlackResults);

        for (int gen = 0; gen < _maxGeneration; gen++)
        {
            std::cout << "---------GENERATION " << gen << "---------" << std::endl;

            whiteParents = {};
            blackParents = {};

            newWhitePopulation = {};
            newBlackPopulation = {};

            for (int j = 0; newWhitePopulation.size() < _N; j++)
            {
                // Select parents
                _tournamentSelection(whiteParents, blackParents, oldResults);

                // Create childs with crossover
                _uniformCrossover(whiteParents, whiteOffspring1, whiteOffspring2);
                _uniformCrossover(blackParents, blackOffspring1, blackOffspring2);

                // Mutate some chromosomes
                _mutate(whiteOffspring1);
                _mutate(whiteOffspring2);
                _mutate(blackOffspring1);
                _mutate(blackOffspring2);

                // Add new childs to the population
                newWhitePopulation.push_back(whiteOffspring1);
                newWhitePopulation.push_back(whiteOffspring2);
                newBlackPopulation.push_back(blackOffspring1);
                newBlackPopulation.push_back(blackOffspring2);
            }

            std::cout << "---------NEW POPULATION CREATED---------" << std::endl;
            results = _fitnessFn.train(newWhitePopulation, newBlackPopulation, _N);

            // reset new Results
            whiteResults = {};
            blackResults = {};

            // Match results with respective child
            _divideResults(results, whiteResults, blackResults, newWhitePopulation, newBlackPopulation);
            _printFitnesses(whiteResults, blackResults);

            // Select the best half of new population and best half of the old population to create the new generation
            _truncationSelection(newWhitePopulation, newBlackPopulation, whiteResults, blackResults);

            // NEW GENERATION CREATED !
            std::cout << "---------NEW GENERATION CREATED---------" << std::endl;
            _printGenes();

            // Set current population as old Population
            oldResults = results;
            _divideResults(oldResults, oldWhiteResults, oldBlackResults, _whitePopulation, _blackPopulation);
        }
    }
};

#endif
