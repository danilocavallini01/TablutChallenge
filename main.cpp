
#include "GA.hpp"
#include "Connectors/Player.hpp"
#include "ResourceAllocator.hpp"

#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace Connectors;

int main(int argc, char *argv[])
{

    ResourceAllocator allocator{};
    allocator.increaseStackSize();

    
    Player p{Player::of("127.0.0.1", "white")};

    p.play();

    /*
    while (true)
    {
        GA geneticAlgorithm = GA(6);
        geneticAlgorithm.start();
    }
    */

    return EXIT_SUCCESS;
}
