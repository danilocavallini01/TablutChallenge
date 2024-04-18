
#include "GA.hpp"

#include <cstdlib>

int main(int argc, char *argv[])
{
    while (true)
    {
        GA geneticAlgorithm = GA(5);
        geneticAlgorithm.start();
    }

    return EXIT_SUCCESS;
}