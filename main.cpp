
#include "GA.hpp"
#include "connectors/Player.hpp"
#include "ResourceAllocator.hpp"

#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace Connectors;

int main(int argc, char *argv[])
{

    ResourceAllocator allocator{};

    allocator.printStackSize();
    allocator.increaseStackSize();
    allocator.printStackSize();
    /*
    Player p{Player::of("127.0.0.1", "white")};
    
    p.play();

    */

    while (true)
    {
        GA geneticAlgorithm = GA(6);
        geneticAlgorithm.start();
    }

    return EXIT_SUCCESS;
}
