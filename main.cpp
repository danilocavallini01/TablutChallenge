
#include "GA.hpp"
#include "Tablut.h"
#include "MoveGenerator.hpp"
#include "Heuristic.hpp"
#include "SearchEngine.h"
#include "Connection.hpp"
#include "ResourceAllocator.hpp"

#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[])
{

   /* Connection c = Connection("IMPOSTOR");
    c.declareName();*/

    
    ResourceAllocator allocator{};

    allocator.increaseStackSize();
    allocator.printSizeDifference();

    
    while (true)
    {
        GA geneticAlgorithm = GA(5);
        geneticAlgorithm.start();
    }
    

    return EXIT_SUCCESS;
}
