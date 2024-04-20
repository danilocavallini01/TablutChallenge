
#include "GA.hpp"

#include <cstdlib>

int main(int argc, char *argv[])
{
    while (true)
    {
        GA geneticAlgorithm = GA(7);
        geneticAlgorithm.start();
    }

    return EXIT_SUCCESS;
}