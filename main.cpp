
#include "GA.hpp"

#include <cstdlib>

int main(int argc, char *argv[])
{
    GA geneticAlgorithm = GA(5);
    geneticAlgorithm.start();

    return EXIT_SUCCESS;
}