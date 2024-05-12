#ifndef GALG
#define GALG

#include "Heuristic.hpp"
#include "Fitness.hpp"

#include <vector>
#include <cstdint>
#include <random>
#include <ctime>
#include <string>
#include <regex>
#include <fstream>
#include <iostream>
#include <filesystem>

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

    std::random_device _rd;

    std::string _fileModelName;
    std::string _backupDir;
    std::string _fitnessBackupDir;

    std::vector<Weights> _whitePopulation;
    std::vector<Weights> _blackPopulation;

    std::vector<std::string> _tokenize(const std::string str, const std::regex re)
    {
        std::sregex_token_iterator it{str.begin(),
                                      str.end(), re, -1};
        std::vector<std::string> tokenized{it, {}};

        // Additional check to remove empty strings
        tokenized.erase(
            std::remove_if(tokenized.begin(),
                           tokenized.end(),
                           [](std::string const &s)
                           {
                               return s.size() == 0;
                           }),
            tokenized.end());

        return tokenized;
    }

    void _readModelFromFile(std::ifstream &__fileModel)
    {
        Weights add;

        std::string line;
        std::regex re(R"([\s|,]+)");
        std::vector<std::string> tokenized;

        for (int i = 0; i < _N; i++)
        {
            getline(__fileModel, line);
            add = {};

            tokenized = _tokenize(line, re);
            int j;

            for (j = 0; j < tokenized.size(); j++)
            {
                add[j] = std::stoi(tokenized.at(j));
            }

            // NOT ENOUGH WEIGTHS IN MODEL -> RANDOMLY GENERATING GENES
            for (j; j < _totalWeights; j++)
            {
                add[j] = _randomGenerateGenes(j);
            }

            _whitePopulation.push_back(add);
        }

        for (int i = 0; i < _N; i++)
        {
            getline(__fileModel, line);
            add = {};

            tokenized = _tokenize(line, re);
            int j;

            for (j = 0; j < tokenized.size(); j++)
            {
                add[j] = std::stoi(tokenized.at(j));
            }

            // NOT ENOUGH WEIGTHS IN MODEL -> RANDOMLY GENERATING GENES
            for (j; j < _totalWeights; j++)
            {
                add[j] = _randomGenerateGenes(j);
            }

            _blackPopulation.push_back(add);
        }
    }

    void _writeModelToFile()
    {
        std::string fileName = std::filesystem::current_path().string() + separator() + _fileModelName;
        _writePopulationToFile(fileName);
    }

    std::string _computeFitnessFileName(std::string &__fileName)
    {
        return std::filesystem::current_path().string() + separator() + _fitnessBackupDir + separator() + __fileName;
    }

    std::string _computeFileName(std::string &__fileName)
    {
        return std::filesystem::current_path().string() + separator() + _backupDir + separator() + __fileName;
    }

    void _backupPopulation()
    {
        std::cout << "BACKUPPING POPULATION" << std::endl;

        // GETTING CURRENT TIMEZONED TIME
        std::time_t result = std::time(nullptr);
        std::string timeStamp = std::ctime(&result);

        // FORMATTING ALL FILES WITH UNDERSCORE
        std::replace(timeStamp.begin(), timeStamp.end(), ':', '_');
        std::replace(timeStamp.begin(), timeStamp.end(), ' ', '_');

        std::string fileName = "LOG_" + _fileModelName + "_" + timeStamp;
        fileName = _computeFileName(fileName);

        _writePopulationToFile(fileName);
    }

    void _backupFitnesses(std::vector<std::pair<Weights, double>> &__whiteResults, std::vector<std::pair<Weights, double>> &__blackResults)
    {
        std::cout << "BACKUPPING FITNESSES" << std::endl;

        // GETTING CURRENT TIMEZONED TIME
        std::time_t result = std::time(nullptr);
        std::string timeStamp = std::ctime(&result);

        // FORMATTING ALL FILES WITH UNDERSCORE
        std::replace(timeStamp.begin(), timeStamp.end(), ':', '_');
        std::replace(timeStamp.begin(), timeStamp.end(), ' ', '_');

        std::string fileName = "LOG_" + _fileModelName + "_" + timeStamp;
        fileName = _computeFitnessFileName(fileName);

        _writeFitnessesToFile(fileName, __whiteResults, __blackResults);
    }

    void _writePopulationToFile(std::string &__fileName)
    {
        std::ofstream fileModel(__fileName, std::ios::trunc);
        std::string line;

        if (!fileModel.is_open())
        {
            std::cout << "UNABLE TO OPEN WRITE FILE -> ABORTING" << std::endl;
            return;
        }

        for (int i = 0; i < _N; i++)
        {
            line = "";
            for (int j = 0; j < _totalWeights; j++)
            {
                line += std::to_string(_whitePopulation.at(i)[j]);
                line += " ";
            }

            fileModel << line << std::endl;
        }

        for (int i = 0; i < _N; i++)
        {
            line = "";
            for (int j = 0; j < _totalWeights; j++)
            {
                line += std::to_string(_blackPopulation.at(i)[j]);
                line += " ";
            }

            fileModel << line << std::endl;
        }
    }

    void _writeFitnessesToFile(std::string &__fileName, std::vector<std::pair<Weights, double>> &__whiteResults, std::vector<std::pair<Weights, double>> &__blackResults)
    {
        std::ofstream fileModel(__fileName, std::ios::trunc);
        std::string line;

        if (!fileModel.is_open())
        {
            std::cout << "UNABLE TO OPEN WRITE FILE -> ABORTING" << std::endl;
            return;
        }

        _writeFitnessToStream(fileModel, __whiteResults, __blackResults);
    }

    void _writeFitnessToStream(std::ostream &out, std::vector<std::pair<Weights, double>> &__whiteResults, std::vector<std::pair<Weights, double>> &__blackResults)
    {
        out << "---------WHITE POPULATION---------" << std::endl;
        for (int i = 0; i < __whiteResults.size(); i++)
        {
            out << "WHITE[" << i << "]: ";
            for (int j = 0; j < _totalWeights; j++)
            {
                out << __whiteResults.at(i).first[j] << ",";
            }

            out << std::endl;
            out << "FITNESS[" << __whiteResults.at(i).second << "]: ";
            out << std::endl;
        }
        out << "---------BLACK POPULATION---------" << std::endl;
        for (int i = 0; i < __blackResults.size(); i++)
        {
            out << "BLACK[" << i << "]: ";
            for (int j = 0; j < _totalWeights; j++)
            {
                out << __blackResults.at(i).first[j] << ",";
            }

            out << std::endl;
            out << "FITNESS[" << __blackResults.at(i).second << "]: ";
            out << std::endl;
        }
        out << "---------END OF BLACK POPULATION---------" << std::endl;
    }

    char separator()
    {
        return std::filesystem::path::preferred_separator;
    }

    void _initalizePopulation()
    {

        // Resetting any previous population
        _whitePopulation = {};
        _blackPopulation = {};

        Weights add;

        // read population model from file if exist
        std::ifstream fileModel(_fileModelName);
        if (fileModel.is_open())
        {
            _readModelFromFile(fileModel);
            return;
        }

        std::cout << "FILE MODEL DOESN'T EXIST -> CREATING RANDOM GENES" << std::endl;

        // Generating random weights from uniform distribution
        for (int i = 0; i < _N; i++)
        {
            add = {};

            for (int j = 0; j < _totalWeights; j++)
            {
                add[j] = _randomGenerateGenes(j);
            }

            _whitePopulation.push_back(add);
        }

        for (int i = 0; i < _N; i++)
        {
            add = {};

            for (int j = 0; j < _totalWeights; j++)
            {

                add[j] = _randomGenerateGenes(j);
            }

            _blackPopulation.push_back(add);
        }
    }

    /*
        Generate a random gene for a specified index within is associated bound
    */
    int _randomGenerateGenes(int __index)
    {
        WeightBounds bound = Heuristic::getWeightBounds(__index);
        std::uniform_int_distribution<int> distribution = std::uniform_int_distribution<int>(bound.first, bound.second);
        return distribution(_rd);
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
        std::uniform_int_distribution<int> distribution(0, _N - 1);

        Weights bestWhitePlayer;
        int bestWhitePlayerScore;

        Weights bestBlackPlayer;
        int bestBlackPlayerScore;

        int selectedWhitePlayer;
        int selectedBlackPlayer;

        for (int i = 0; i < _tournSize; i++)
        {
            // CHOOSE RANDOMLY FROM THE POPULATION
            selectedWhitePlayer = distribution(_rd);
            selectedBlackPlayer = distribution(_rd);

            if (i == 0 || __fitnesses[selectedWhitePlayer].first > bestWhitePlayerScore)
            {
                bestWhitePlayer = _whitePopulation[selectedWhitePlayer];
                bestWhitePlayerScore = __fitnesses[selectedWhitePlayer].first;
            }

            if (i == 0 || __fitnesses[selectedBlackPlayer].second > bestBlackPlayerScore)
            {
                bestBlackPlayer = _blackPopulation[selectedBlackPlayer];
                bestBlackPlayerScore = __fitnesses[selectedBlackPlayer].second;
            }
        }

        __whiteParent = bestWhitePlayer;
        __blackParent = bestBlackPlayer;
    }

    /*
        In uniform crossover, each bit is chosen from either parent with equal probability.
        In this, we essentially flip a coin for each chromosome to decide whether or not it will be included in the off-spring.
    */

    void _uniformCrossover(std::pair<Weights, Weights> &__parents, Weights &__offspring1, Weights &__offspring2)
    {
        std::uniform_int_distribution<int> coinToss(0, 1);

        // CROSSOVER START
        for (int i = 0; i < _totalWeights; i++)
        {
            // Toss coin
            if (coinToss(_rd) == 1)
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
        Mutate some gene of the offspring with probability -> _mutationProb
        Mutation change the value of the selected gene by max +- _mutationFactor / 2
    */
    void _mutate(Weights &__offspring)
    {
        std::uniform_real_distribution<double> mutationProbDistribution(0, 1.0);

        for (int i = 0; i < _totalWeights; i++)
        {
            if (mutationProbDistribution(_rd) <= _mutationProb)
            {
                __offspring[i] = _mutateOffspring(__offspring[i], i);
            }
        }
    }

    /*
        Mutate a single gene of offspring by checking the bound of the specified Weight and by changing it
        by max +-10% of its bound
    */
    int _mutateOffspring(int __gene, int __index)
    {
        WeightBounds bound = Heuristic::getWeightBounds(__index);
        int boundDifference = std::abs((bound.second - bound.first) / _mutationFactor);
        std::uniform_int_distribution<int> distribution{std::uniform_int_distribution<int>(-boundDifference, boundDifference)};

        __gene += distribution(_rd);

        if (__gene < bound.first)
        {
            return bound.first;
        }
        if (__gene > bound.second)
        {
            return bound.second;
        }

        return __gene;
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
        _writeFitnessToStream(std::cout, __whiteResults, __blackResults);
    }

public:
    GA(int __maxDepth = 7, bool __verbose = true, int __N = 10, double __mutationProb = 0.15,
       int __mutationFactor = 5, int __tournSize = 3, int __generation = 10) : _N(__N),
                                                                               _mutationProb(__mutationProb),
                                                                               _mutationFactor(100 / __mutationFactor),
                                                                               _tournSize(__tournSize),
                                                                               _maxGeneration(__generation),
                                                                               _totalWeights(TOTAL_WEIGHTS),
                                                                               _fitnessFn(Fitness(__maxDepth, 250, __verbose)),
                                                                               _fileModelName("tablutGaModel.model"),
                                                                               _backupDir("backup"),
                                                                               _fitnessBackupDir("fitness")
    {

        std::random_device _rd{"/dev/urandom"};
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

            whiteOffspring1 = {};
            whiteOffspring2 = {};

            blackOffspring1 = {};
            blackOffspring2 = {};

            while (newWhitePopulation.size() < _N)
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
            _backupPopulation();
            _backupFitnesses(whiteResults, blackResults);

            // Set current population as old Population
            oldResults = results;
            _divideResults(oldResults, oldWhiteResults, oldBlackResults, _whitePopulation, _blackPopulation);
        }

        _writeModelToFile();
    }
};

#endif
