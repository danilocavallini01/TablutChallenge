
#include "GA.hpp"
#include "Tablut.h"
#include "MoveGenerator.hpp"
#include "Heuristic.hpp"
#include "SearchEngine.h"

#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[])
{
    // customMatch();

    while (true)
    {
        GA geneticAlgorithm = GA(6);
        geneticAlgorithm.start();
    }

    return EXIT_SUCCESS;
}
