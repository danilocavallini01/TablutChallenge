
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

    /*
    Player p{Player::of("127.0.0.1", "white")};

    p.play();
*/
    std::chrono::steady_clock::time_point timeBegin, timeEnd;
    Tablut gameBoard = Tablut::getStartingPosition();
    std::vector<Tablut> pos = {};

    timeBegin = std::chrono::steady_clock::now();
    MoveGenerator::countLegalMoves(gameBoard);
    timeEnd = std::chrono::steady_clock::now();
    // STAT ------------------

    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(timeEnd - timeBegin).count() << std::endl;

    while (true)
    {
        GA geneticAlgorithm = GA(6);
        geneticAlgorithm.start();
    }

    return EXIT_SUCCESS;
}
